#include "Engine/Console.h"
#include "Engine/Logging.h"
#include <cstdio>

#if UMBRAL_PLATFORM_IS_WINDOWS
#	include "Engine/Windows/WindowsConsole.h"
#else
#	include "Engine/Posix/PosixConsole.h"
#endif

static EConsoleColor GConsoleBackgroundColor = EConsoleColor::Default;
static EConsoleColor GConsoleForegroundColor = EConsoleColor::Default;
static EConsoleTextStyle GConsoleTextStyle = EConsoleTextStyle::None;

static inline void WriteStringViewToStdOut(const FStringView text)
{
	static_assert(IsSame<char, FStringView::CharType>);

	::fwrite(text.GetChars(), sizeof(char), static_cast<size_t>(text.Length()), stdout);
}

void FConsole::Flush()
{
	::fflush(stdout);
}

EConsoleColor FConsole::GetBackgroundColor()
{
	return GConsoleBackgroundColor;
}

EConsoleColor FConsole::GetForegroundColor()
{
	return GConsoleForegroundColor;
}

EConsoleTextStyle FConsole::GetTextStyle()
{
	return GConsoleTextStyle;
}

FString::CharType FConsole::ReadChar()
{
	const int32 ch = ::fgetc(stdin);

	if (::feof(stdin) || ::ferror(stdin))
	{
		return FStringView::CharTraitsType::NullChar;
	}

	return static_cast<FString::CharType>(ch);
}

FString FConsole::ReadLine()
{
	return FPlatformConsole::ReadLine();
}

void FConsole::ResetAttributes()
{
	// https://learn.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences#text-formatting

	// 0 = Reset all attributes to default state
	// 22 = No bold
	// 24 = No underline
	// 27 = Positive (don't swap foreground and background)
	// 39 = Default foreground
	// 49 = Default background

	FConsole::Write("\x1b[0m\x1b[22m\x1b[24m\x1b[27m\x1b[39m\x1b[49m"_sv);
}

void FConsole::SetBackgroundColor(const EConsoleColor color)
{
	if (GConsoleBackgroundColor == color)
	{
		return;
	}

	GConsoleBackgroundColor = color;

	const FStringView backgroundSequence = []()
	{
		// https://learn.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences#text-formatting

		switch (GConsoleBackgroundColor)
		{
		case EConsoleColor::Default:        return "\x1b[49m"_sv;
		case EConsoleColor::Black:          return "\x1b[40m"_sv;
		case EConsoleColor::Red:            return "\x1b[41m"_sv;
		case EConsoleColor::Green:          return "\x1b[42m"_sv;
		case EConsoleColor::Yellow:         return "\x1b[43m"_sv;
		case EConsoleColor::Blue:           return "\x1b[44m"_sv;
		case EConsoleColor::Magenta:        return "\x1b[45m"_sv;
		case EConsoleColor::Cyan:           return "\x1b[46m"_sv;
		case EConsoleColor::White:          return "\x1b[47m"_sv;
		case EConsoleColor::BrightBlack:    return "\x1b[100m"_sv;
		case EConsoleColor::BrightRed:      return "\x1b[101m"_sv;
		case EConsoleColor::BrightGreen:    return "\x1b[102m"_sv;
		case EConsoleColor::BrightYellow:   return "\x1b[103m"_sv;
		case EConsoleColor::BrightBlue:     return "\x1b[104m"_sv;
		case EConsoleColor::BrightMagenta:  return "\x1b[105m"_sv;
		case EConsoleColor::BrightCyan:     return "\x1b[106m"_sv;
		case EConsoleColor::BrightWhite:    return "\x1b[107m"_sv;
		}

		UM_ASSERT_NOT_REACHED();
	}();

	WriteStringViewToStdOut(backgroundSequence);
}

void FConsole::SetForegroundColor(const EConsoleColor color)
{
	if (GConsoleForegroundColor == color)
	{
		return;
	}

	GConsoleForegroundColor = color;

	const FStringView foregroundSequence = []()
	{
		// https://learn.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences#text-formatting

		switch (GConsoleForegroundColor)
		{
		case EConsoleColor::Default:        return "\x1b[39m"_sv;
		case EConsoleColor::Black:          return "\x1b[30m"_sv;
		case EConsoleColor::Red:            return "\x1b[31m"_sv;
		case EConsoleColor::Green:          return "\x1b[32m"_sv;
		case EConsoleColor::Yellow:         return "\x1b[33m"_sv;
		case EConsoleColor::Blue:           return "\x1b[34m"_sv;
		case EConsoleColor::Magenta:        return "\x1b[35m"_sv;
		case EConsoleColor::Cyan:           return "\x1b[36m"_sv;
		case EConsoleColor::White:          return "\x1b[37m"_sv;
		case EConsoleColor::BrightBlack:    return "\x1b[90m"_sv;
		case EConsoleColor::BrightRed:      return "\x1b[91m"_sv;
		case EConsoleColor::BrightGreen:    return "\x1b[92m"_sv;
		case EConsoleColor::BrightYellow:   return "\x1b[93m"_sv;
		case EConsoleColor::BrightBlue:     return "\x1b[94m"_sv;
		case EConsoleColor::BrightMagenta:  return "\x1b[95m"_sv;
		case EConsoleColor::BrightCyan:     return "\x1b[96m"_sv;
		case EConsoleColor::BrightWhite:    return "\x1b[97m"_sv;
		}

		UM_ASSERT_NOT_REACHED();
	}();

	WriteStringViewToStdOut(foregroundSequence);
}

void FConsole::SetTextStyle(const EConsoleTextStyle style)
{
	const bool addBold         = HasFlag(GConsoleTextStyle, EConsoleTextStyle::Bold) == false && HasFlag(style, EConsoleTextStyle::Bold);
	const bool removeBold      = HasFlag(GConsoleTextStyle, EConsoleTextStyle::Bold) && HasFlag(style, EConsoleTextStyle::Bold) == false;
	const bool addUnderline    = HasFlag(GConsoleTextStyle, EConsoleTextStyle::Underline) == false && HasFlag(style, EConsoleTextStyle::Underline);
	const bool removeUnderline = HasFlag(GConsoleTextStyle, EConsoleTextStyle::Underline) && HasFlag(style, EConsoleTextStyle::Underline) == false;

	// https://learn.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences#text-formatting

	if (addBold)
	{
		WriteStringViewToStdOut("\x1b[1m"_sv);
	}
	else if (removeBold)
	{
		WriteStringViewToStdOut("\x1b[22m"_sv);
	}

	if (addUnderline)
	{
		WriteStringViewToStdOut("\x1b[4m"_sv);
	}
	else if (removeUnderline)
	{
		WriteStringViewToStdOut("\x1b[24m"_sv);
	}

	GConsoleTextStyle = style;
}

void FConsole::Write(const FStringView text)
{
	WriteStringViewToStdOut(text);

#if UMBRAL_PLATFORM_IS_WINDOWS
	Flush();
#endif
}

void FConsole::WriteLine(const FStringView text)
{
	WriteStringViewToStdOut(text);
	WriteStringViewToStdOut("\n"_sv);

#if UMBRAL_PLATFORM_IS_WINDOWS
	Flush();
#endif
}

void FConsole::WriteLine()
{
	WriteStringViewToStdOut("\n"_sv);

#if UMBRAL_PLATFORM_IS_WINDOWS
	Flush();
#endif

}

FScopedConsoleBackgroundColor::FScopedConsoleBackgroundColor(const EConsoleColor newColor)
	: m_BackgroundColor { FConsole::GetBackgroundColor() }
{
	FConsole::SetBackgroundColor(newColor);
}

FScopedConsoleBackgroundColor::~FScopedConsoleBackgroundColor()
{
	FConsole::SetBackgroundColor(m_BackgroundColor);
}

FScopedConsoleForegroundColor::FScopedConsoleForegroundColor(const EConsoleColor newColor)
	: m_ForegroundColor { FConsole::GetForegroundColor() }
{
	FConsole::SetForegroundColor(newColor);
}

FScopedConsoleForegroundColor::~FScopedConsoleForegroundColor()
{
	FConsole::SetForegroundColor(m_ForegroundColor);
}