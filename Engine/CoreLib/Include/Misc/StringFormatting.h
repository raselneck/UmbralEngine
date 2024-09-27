#pragma once

#include "Containers/Array.h"
#include "Containers/StringView.h"
#include "Containers/Variant.h"
#include "Memory/UniquePtr.h"
#include "Templates/Decay.h"
#include "Templates/IsPointer.h"
#include "Templates/IsReference.h"
#include "Templates/IsZeroConstructible.h"
#include "Templates/VariadicTraits.h"

class FString;
class FStringBuilder;

/**
 * @brief Defines a way to custom format types as strings.
 *
 * See the TFormatter implementation for the math classes for examples.
 *
 * @tparam T The type to format.
 */
template<typename T>
struct TFormatter
{
	void BuildString(const T& value, FStringBuilder& builder);
	bool Parse(FStringView formatString);
};

namespace Private
{
	/**
	 * @brief Defines the interface to use for custom type formatters.
	 */
	class ITypeFormatter
	{
	public:

		virtual ~ITypeFormatter() = default;
		virtual void BuildString(FStringBuilder& builder) = 0;
		virtual bool Parse(FStringView formatString) = 0;
	};

	/**
	 * @brief Defines a custom string formatter instance.
	 *
	 * @tparam T The type of the value being formatted.
	 */
	template<typename T>
	class TTypeFormatter final : public ITypeFormatter
	{
	public:

		using ElementType = T;
		using FormatterType = TFormatter<ElementType>;

		/**
		 * @brief Creates a new type formatter instance.
		 *
		 * @param value The value to format.
		 */
		explicit TTypeFormatter(const ElementType& value)
			: m_Value { value }
		{
		}

		/**
		 * @brief Creates a new type formatter instance.
		 *
		 * @param value The value to format.
		 */
		explicit TTypeFormatter(ElementType&& value)
			: m_Value { MoveTemp(value) }
		{
		}

		/**
		 * @brief Appends the characters to the given array necessary to represent the underlying value.
		 *
		 * @param chars The character array to append to.
		 */
		virtual void BuildString(FStringBuilder& builder) override
		{
			m_Formatter.BuildString(m_Value, builder);
		}

		/**
		 * @brief Parses the given format string for formatting options.
		 *
		 * @param formatString The format string.
		 * @return True if the format string was successfully parsed, otherwise false.
		 */
		virtual bool Parse(const FStringView formatString) override
		{
			return m_Formatter.Parse(formatString);
		}

	private:

		ElementType m_Value;
		FormatterType m_Formatter;
	};

	/**
	 * @brief Defines a string formatting argument.
	 */
	class FStringFormatArgument final
	{
		UM_DISABLE_COPY(FStringFormatArgument);

	public:

		using ValueType = TVariant<FEmptyType, char, int64, uint64, float, double, FStringView, void*, bool, TUniquePtr<ITypeFormatter>>;

		/**
		 * @brief Sets default values for this string format argument.
		 */
		FStringFormatArgument();

		// Formatting arguments are move-able
		FStringFormatArgument(FStringFormatArgument&&) = default;
		FStringFormatArgument& operator=(FStringFormatArgument&&) = default;

		/**
		 * @brief Destroys this string format argument.
		 */
		~FStringFormatArgument();

		/**
		 * @brief Creates a new string formatting argument wrapping the given value.
		 *
		 * @param value The value.
		 */
		explicit FStringFormatArgument(char value);

		/**
		 * @brief Creates a new string formatting argument wrapping the given value.
		 *
		 * @param value The value.
		 */
		explicit FStringFormatArgument(const int8 value)
		{
			m_Value.ResetToType<int64>(value);
		}

		/**
		 * @brief Creates a new string formatting argument wrapping the given value.
		 *
		 * @param value The value.
		 */
		explicit FStringFormatArgument(const int16 value)
		{
			m_Value.ResetToType<int64>(value);
		}

		/**
		 * @brief Creates a new string formatting argument wrapping the given value.
		 *
		 * @param value The value.
		 */
		explicit FStringFormatArgument(const int32 value)
		{
			m_Value.ResetToType<int64>(value);
		}

		/**
		 * @brief Creates a new string formatting argument wrapping the given value.
		 *
		 * @param value The value.
		 */
		explicit FStringFormatArgument(const int64 value)
		{
			m_Value.ResetToType<int64>(value);
		}

		/**
		 * @brief Creates a new string formatting argument wrapping the given value.
		 *
		 * @param value The value.
		 */
		explicit FStringFormatArgument(const uint8 value)
		{
			m_Value.ResetToType<uint64>(value);
		}

		/**
		 * @brief Creates a new string formatting argument wrapping the given value.
		 *
		 * @param value The value.
		 */
		explicit FStringFormatArgument(const uint16 value)
		{
			m_Value.ResetToType<uint64>(value);
		}

		/**
		 * @brief Creates a new string formatting argument wrapping the given value.
		 *
		 * @param value The value.
		 */
		explicit FStringFormatArgument(const uint32 value)
		{
			m_Value.ResetToType<uint64>(value);
		}

		/**
		 * @brief Creates a new string formatting argument wrapping the given value.
		 *
		 * @param value The value.
		 */
		explicit FStringFormatArgument(const uint64 value)
		{
			m_Value.ResetToType<uint64>(value);
		}

#if UMBRAL_PLATFORM_LONG_IS_UNIQUE
		explicit FStringFormatArgument(const signed long int value)
		{
			m_Value.ResetToType<int64>(value);
		}
		explicit FStringFormatArgument(const unsigned long int value)
		{
			m_Value.ResetToType<uint64>(value);
		}
#endif

#if UMBRAL_PLATFORM_LONG_LONG_IS_UNIQUE
		explicit FStringFormatArgument(const signed long long value)
		{
			m_Value.ResetToType<int64>(value);
		}
		explicit FStringFormatArgument(const unsigned long long value)
		{
			m_Value.ResetToType<uint64>(value);
		}
#endif

		/**
		 * @brief Creates a new string formatting argument wrapping the given value.
		 *
		 * @param value The value.
		 */
		explicit FStringFormatArgument(const float value)
		{
			m_Value.ResetToType<float>(value);
		}

		/**
		 * @brief Creates a new string formatting argument wrapping the given value.
		 *
		 * @param value The value.
		 */
		explicit FStringFormatArgument(const double value)
		{
			m_Value.ResetToType<double>(value);
		}

		/**
		 * @brief Creates a new string formatting argument wrapping the given value.
		 *
		 * @param value The value.
		 */
		explicit FStringFormatArgument(const FString& value);

		/**
		 * @brief Creates a new string formatting argument wrapping the given value.
		 *
		 * @param value The value.
		 */
		explicit FStringFormatArgument(FString&& value);

		/**
		 * @brief Creates a new string formatting argument wrapping the given value.
		 *
		 * @param value The value.
		 */
		explicit FStringFormatArgument(FStringView value);

		/**
		 * @brief Creates a new string formatting argument wrapping the given value.
		 *
		 * @param value The value.
		 */
		explicit FStringFormatArgument(const char* value)
			: FStringFormatArgument(FStringView { value })
		{
		}

		/**
		 * @brief Creates a new string formatting argument wrapping the given value.
		 *
		 * @param value The value.
		 */
		explicit FStringFormatArgument(const bool value)
		{
			m_Value.ResetToType<bool>(value);
		}

		/**
		 * @brief Creates a new string formatting argument wrapping the given value.
		 *
		 * @tparam T The type of the value.
		 * @param value The value.
		 */
		explicit FStringFormatArgument(void* value)
		{
			m_Value.ResetToType<void*>(value);
		}

		/**
		 * @brief Creates a new string formatting argument wrapping the given value.
		 *
		 * @tparam T The type of the value.
		 * @param value The value.
		 */
		template<typename T>
		explicit FStringFormatArgument(const T& value)
		{
			using ElementType = typename TDecay<T>::Type;
			m_Value.ResetToType<TUniquePtr<ITypeFormatter>>(MakeUnique<TTypeFormatter<ElementType>>(value));
		}

		/**
		 * @brief Creates a new string formatting argument wrapping the given value.
		 *
		 * @tparam T The type of the value.
		 * @param value The value.
		 */
		template<typename T>
		explicit FStringFormatArgument(T&& value)
		{
			using ElementType = typename TDecay<T>::Type;
			m_Value.ResetToType<TUniquePtr<ITypeFormatter>>(MakeUnique<TTypeFormatter<ElementType>>(MoveTemp(value)));
		}

		/**
		 * @brief Creates a new string formatting argument wrapping the given value.
		 *
		 * @tparam T The type of the value.
		 * @param value The value.
		 */
		template<typename T>
		explicit FStringFormatArgument(const T* value)
			: FStringFormatArgument(const_cast<void*>(static_cast<const void*>(value)))
		{
		}

		/**
		 * @brief Creates a new string formatting argument wrapping the given value.
		 *
		 * @tparam T The type of the value.
		 * @param value The value.
		 */
		template<typename T>
		explicit FStringFormatArgument(T* value)
			: FStringFormatArgument(static_cast<void*>(value))
		{
		}

		/**
		 * @brief Builds the string for the underlying value.
		 *
		 * @param formatString The format string to parse.
		 * @param builder The string builder to append to.
		 */
		void BuildString(FStringView formatString, FStringBuilder& builder);

	private:

		ValueType m_Value;
	};

	/**
	 * @brief Makes an array of string formatting arguments from supplied arguments.
	 *
	 * @tparam ArgTypes The types of the arguments.
	 * @param args The arguments.
	 * @return The array of string formatting arguments.
	 */
	template<typename... ArgTypes>
	[[nodiscard]] TArray<FStringFormatArgument> MakeFormatArgumentArray(ArgTypes&&... args)
	{
		TArray<FStringFormatArgument> result;
		result.Reserve(static_cast<int32>(sizeof...(args)));

		([&]()
		{
			(void)result.Emplace(Forward<ArgTypes>(args));
		}(), ...);

		return result;
	}
}

template<>
struct TIsZeroConstructible<Private::FStringFormatArgument> : TIsZeroConstructible<Private::FStringFormatArgument::ValueType>
{
};