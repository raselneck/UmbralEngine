#include "Containers/StringView.h"
#include "Engine/Internationalization.h"
#include "Engine/Logging.h"
#include "HAL/Directory.h"
#include "HAL/Path.h"
#include "Memory/Memory.h"
#include "Misc/Unicode.h"
#include "Templates/CharTraits.h"

#include <clocale>
#include <unicode/ucasemap.h>
#include <unicode/uclean.h>
#include <unicode/ucol.h>
#include <unicode/utrace.h>
#include <unicode/utypes.h>

using FConversionFunction = int32(*)(const UCaseMap* csm,
                                     char* dest,
                                     int32 destCapacity,
                                     const char* src,
                                     int32 srcLength,
                                     UErrorCode* pErrorCode);

static FStringView GLocale;
static UCaseMap* GCaseMap = nullptr;
static UCollator* GCollator = nullptr;

namespace ICU
{
	static void* Alloc(const void* /*context*/, const size_t size)
	{
		return FMemory::Allocate(static_cast<FMemory::SizeType>(size));
	}

	static void* Realloc(const void* /*context*/, void* memory, const size_t size)
	{
		return FMemory::Reallocate(memory, static_cast<FMemory::SizeType>(size));
	}

	static void Free(const void* /*context*/, void* memory)
	{
		FMemory::Free(memory);
	}

	static void U_CALLCONV TraceCallback(const void* /*context*/, int32 functionNumber, int32 traceLevel, const char* fmt, va_list args)
	{
		if (UM_ENSURE(traceLevel != UTRACE_OFF) == false)
		{
			return;
		}

		char buffer[1024];
		const int32 bufferSize = UM_ARRAY_SIZE(buffer);

		const FStringView functionName { utrace_functionName(functionNumber) };
		const int32 messageLength = utrace_vformat(buffer, bufferSize, 0, fmt, args);
		const FStringView message { buffer, messageLength };

		switch (traceLevel)
		{
		/** Trace error conditions only */
		case UTRACE_ERROR:
			UM_LOG(Error, "[ICU] {} {}", functionName, message);
			break;

		/** Trace errors and warnings */
		case UTRACE_WARNING:
			UM_LOG(Warning, "[ICU] {} {}", functionName, message);
			break;

		/** Trace opens and closes of ICU services  @stable ICU 2.8*/
		case UTRACE_OPEN_CLOSE:
		/** Trace an intermediate number of ICU operations  @stable ICU 2.8*/
		case UTRACE_INFO:
			UM_LOG(Info, "[ICU] {} {}", functionName, message);
			break;

		/** Trace the maximum number of ICU operations  @stable ICU 2.8*/
		case UTRACE_VERBOSE:
		default:
			UM_LOG(Debug, "[ICU] {} {}", functionName, message);
			break;
		}
	}
}

static TArray<char> ConvertStringOrGrowAndTryAgain(FConversionFunction convert, FInternationalization::CharSpanType stringToConvert)
{
	int32 resultLength = 0;
	TArray<char> resultChars;

	do
	{
		UErrorCode convertStatus = U_ZERO_ERROR;
		resultLength = convert(GCaseMap,
		                       resultChars.GetData(),
		                       resultChars.Num(),
		                       stringToConvert.GetData(),
		                       stringToConvert.Num(),
		                       &convertStatus);

		if (U_SUCCESS(convertStatus))
		{
			break;
		}

		UM_LOG(Verbose, "Failed to convert string with error {}; attempting to grow", u_errorName(convertStatus));

		resultChars.Reset();
		resultChars.Reserve(resultLength + 1);
		resultChars.AddZeroed(resultLength + 1);
	} while (true);

	if (LIKELY(resultChars.Num() > 0))
	{
		// +1 for the null terminator
		UM_ENSURE(resultChars.Num() == resultLength + 1);
		UM_ENSURE(resultChars.Last() == TCharTraits<char>::NullChar);
	}

	return resultChars;
}

static constexpr bool IsCaseSensitiveComparison(const EStringComparison comparison)
{
	switch (comparison)
	{
	case EStringComparison::CurrentCulture:
	case EStringComparison::InvariantCulture:
	case EStringComparison::Ordinal:
		return true;

	default:
		return false;
	}
}

TErrorOr<ECompareResult> FInternationalization::CompareStrings(const CharSpanType first, const CharSpanType second, const EStringComparison comparison)
{
	if (GCollator == nullptr || comparison == EStringComparison::Ordinal || comparison == EStringComparison::OrdinalIgnoreCase)
	{
		// TODO Log here if GCollator is null?

		// Kinda hacky, but EIgnoreCase inherits from bool, so...
		const EIgnoreCase ignoreCase = static_cast<EIgnoreCase>(IsCaseSensitiveComparison(comparison));
		return Private::OrdinalCompareCharSpans(first, second, ignoreCase);
	}

	// L1 = letters             = UCOL_PRIMARY
	// L2 = L1 + accents        = UCOL_SECONDARY
	// L3 = L2 + case           = UCOL_TERTIARY
	// L4 = L3 + punctuation    = UCOL_QUATERNARY
	// L5 = L4 + codepoint      = UCOL_IDENTICAL
	const UCollationStrength strength = [comparison]() -> UCollationStrength
	{
		// TODO Need to make a distinction between current and invariant culture
		switch (comparison)
		{
		case EStringComparison::CurrentCulture:
		case EStringComparison::InvariantCulture:
			return UCOL_TERTIARY;

		case EStringComparison::CurrentCultureIgnoreCase:
		case EStringComparison::InvariantCultureIgnoreCase:
			return UCOL_SECONDARY;

		default:
			UM_ASSERT_NOT_REACHED_MSG("Unhandled string comparison type");
		}
	}();

	ucol_setStrength(GCollator, strength);

	UErrorCode status = U_ZERO_ERROR;
	const UCollationResult result = ucol_strcollUTF8(GCollator,
	                                                 first.GetData(),
	                                                 first.Num(),
	                                                 second.GetData(),
	                                                 second.Num(),
	                                                 &status);
	UM_ENSURE(U_SUCCESS(status));

	switch (result)
	{
	case UCOL_GREATER:
		return ECompareResult::GreaterThan;

	case UCOL_LESS:
		return ECompareResult::LessThan;

	case UCOL_EQUAL:
		return ECompareResult::Equals;

	default:
		return MAKE_ERROR("Unknown collation result \"{}\"", static_cast<int32>(result));
	}
}

TArray<char> FInternationalization::ConvertStringToLower(const CharSpanType string)
{
	TArray<char> result;
	if (string.IsEmpty())
	{
		return result;
	}

	result = ConvertStringOrGrowAndTryAgain(ucasemap_utf8ToLower, string);
	UM_ENSURE(result.Last() == '\0');

	return result;
}

TArray<char> FInternationalization::ConvertStringToUpper(const CharSpanType string)
{
	TArray<char> result;
	if (string.IsEmpty())
	{
		return result;
	}

	result = ConvertStringOrGrowAndTryAgain(ucasemap_utf8ToUpper, string);
	UM_ENSURE(result.Last() == '\0');

	return result;
}

FStringView FInternationalization::GetLocale()
{
	UM_ENSURE(GCaseMap != nullptr);
	return FStringView { ucasemap_getLocale(GCaseMap) };
}

TErrorOr<void> FInternationalization::Initialize()
{
	UErrorCode status = U_ZERO_ERROR;
	u_setMemoryFunctions(nullptr, ICU::Alloc, ICU::Realloc, ICU::Free, &status);
	if (U_FAILURE(status))
	{
		return MAKE_ERROR("Failed to set ICU memory functions; reason: {}", u_errorName(status));
	}

	utrace_setFunctions(nullptr, nullptr, nullptr, ICU::TraceCallback);
	utrace_setLevel(UTRACE_VERBOSE);

	const FString currentDirectory = FDirectory::GetExecutableDir();
	const FString icuDirectory = FPath::Join(currentDirectory, "ICU"_sv);
	u_setDataDirectory(icuDirectory.GetChars());

#if 0
	u_init(&status);
	if (U_FAILURE(status))
	{
		return MAKE_ERROR("Failed to initialize ICU; reason: {}", u_errorName(status));
	}
#endif

	std::setlocale(LC_ALL, ""); // Set the locale to be the system's desired locale

	GLocale = FStringView { std::setlocale(LC_ALL, nullptr) };
	UM_LOG(Verbose, "The system locale is `{}'", GLocale);

	GCaseMap = ucasemap_open(GLocale.GetChars(), 0, &status);
	if (U_FAILURE(status))
	{
		return MAKE_ERROR("Failed to open case map for locale `{}'; reason: {}", GLocale, u_errorName(status));
	}

	GCollator = ucol_open(GLocale.GetChars(), &status);
	if (U_FAILURE(status))
	{
		// TODO This should really be a returned error. To fix it, though, we'd need to copy ICU's config files somehow
		UM_LOG(Error, "Failed to open collator for locale `{}'; reason: {}", GLocale, u_errorName(status));
	}

	return {};
}

TErrorOr<void> FInternationalization::SetLocale(const FStringView locale)
{
	if (GCollator)
	{
		ucol_close(GCollator);
		GCollator = nullptr;
	}

	UErrorCode status = U_ZERO_ERROR;
	ucasemap_setLocale(GCaseMap, locale.GetChars(), &status);
	if (U_FAILURE(status))
	{
		return MAKE_ERROR("Failed to set locale `{}' on case map; reason: {}", locale, u_errorName(status));
	}

	GCollator = ucol_open(locale.GetChars(), &status);
	if (U_FAILURE(status))
	{
		return MAKE_ERROR("Failed to open collator for locale `{}'; reason: {}", locale, u_errorName(status));
	}

	return {};
}

void FInternationalization::Shutdown()
{
	if (GCollator)
	{
		ucol_close(GCollator);
		GCollator = nullptr;
	}

	if (GCaseMap)
	{
		ucasemap_close(GCaseMap);
		GCaseMap = nullptr;
	}
}