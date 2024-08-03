#pragma once

#include "Containers/String.h"
#include "Containers/StringView.h"
#include "Containers/Variant.h"
#include "Templates/Decay.h"

/**
 * @brief Defines a reference to a string that can help avoid use-after-free issues.
 */
class FStringReference
{
public:

	using CharType = typename FStringView::CharType;
	using SizeType = typename FStringView::SizeType;
	using SpanType = typename FStringView::SpanType;
	using VariantType = TVariant<FStringView, FString>;

	/**
	 * @brief Creates a string reference from a string literal.
	 *
	 * @tparam N The number of characters in the string literal.
	 * @param value The string literal.
	 * @return The string reference.
	 */
	template<SizeType N>
	[[nodiscard]] static FStringReference FromLiteral(const CharType (&value)[N])
	{
		return FromStringView(FStringView { value, N });
	}

	/**
	 * @brief Creates a string reference from a string.
	 *
	 * @param value The string.
	 * @return The string reference.
	 */
	[[nodiscard]] static FStringReference FromString(const FString& value);

	/**
	 * @brief Creates a string reference from a string.
	 *
	 * @param value The string.
	 * @return The string reference.
	 */
	[[nodiscard]] static FStringReference FromString(FString&& value);

	/**
	 * @brief Creates a string reference from a string view.
	 *
	 * @param value The string view.
	 * @return The string reference.
	 */
	[[nodiscard]] static FStringReference FromStringView(FStringView value);

	/**
	 * @brief Gets the underlying string's characters.
	 *
	 * @return The underlying string's characters.
	 */
	[[nodiscard]] const CharType* GetChars() const;

	/**
	 * @brief Gets the underlying string's length.
	 *
	 * @return The underlying string's length.
	 */
	[[nodiscard]] SizeType Length() const;

	/**
	 * @brief Converts this string reference to a string.
	 *
	 * @return This string reference as a string.
	 */
	[[nodiscard]] FString ToString() const;

	/**
	 * @brief Gets this string reference as a string view.
	 *
	 * @return This string reference as a string view.
	 */
	[[nodiscard]] operator FStringView() const;

	/**
	 * @brief Gets this string reference as a character span.
	 *
	 * @return This string reference as a character span.
	 */
	[[nodiscard]] operator SpanType() const;

private:

	VariantType m_Value;
};

namespace Private
{
	template<typename T>
	struct TStringReferenceHelper
	{
	};

	template<>
	struct TStringReferenceHelper<const FStringReference::CharType*>
	{
		template<int32 N>
		static FStringReference Make(const FStringReference::CharType (&value)[N])
		{
			return FStringReference::FromLiteral(value);
		}
	};

	template<>
	struct TStringReferenceHelper<FString>
	{
		static FStringReference Make(const FString& value)
		{
			return FStringReference::FromString(value);
		}

		static FStringReference Make(FString&& value)
		{
			return FStringReference::FromString(MoveTemp(value));
		}
	};

	template<>
	struct TStringReferenceHelper<FStringView>
	{
		static FStringReference Make(FStringView value)
		{
			return FStringReference::FromStringView(MoveTemp(value));
		}
	};
}

template<typename T>
FStringReference MakeStringReference(T value)
{
	using Type = typename TDecay<T>::Type;
	return ::Private::TStringReferenceHelper<Type>::Make(MoveTemp(value));
}