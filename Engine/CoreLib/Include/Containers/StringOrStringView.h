#pragma once

#include "Containers/String.h"
#include "Containers/StringView.h"
#include "Containers/Variant.h"
#include "Templates/Decay.h"

/**
 * @brief Defines a container for either a string or string view, which can be used to help prevent use-after-free issues with strings.
 */
class FStringOrStringView
{
public:

	using CharType = typename FStringView::CharType;
	using SizeType = typename FStringView::SizeType;
	using SpanType = typename FStringView::SpanType;
	using VariantType = TVariant<FStringView, FString>;

	/**
	 * @brief Creates a wrapper for a null-terminated C string.
	 *
	 * @param value The null-terminated C string.
	 */
	explicit FStringOrStringView(const CharType* value);

	/**
	 * @brief Creates a wrapper for a raw C string.
	 *
	 * @param value The raw C string.
	 * @param length The length of the C string.
	 */
	FStringOrStringView(const CharType* value, SizeType length);

	/**
	 * @brief Creates a wrapper for a character array.
	 *
	 * @tparam N The number of characters in the array.
	 * @param value The character array.
	 */
	template<SizeType N>
	explicit FStringOrStringView(const CharType (&value)[N])
		: FStringOrStringView(FStringView { value, N })
	{
	}

	/**
	 * @brief Creates a wrapper for a character span.
	 *
	 * @param value The character span.
	 */
	explicit FStringOrStringView(SpanType value);

	/**
	 * @brief Creates a wrapper for a string.
	 *
	 * @param value The string.
	 */
	explicit FStringOrStringView(const FString& value);

	/**
	 * @brief Creates a wrapper for a string.
	 *
	 * @param value The string.
	 */
	explicit FStringOrStringView(FString&& value);

	/**
	 * @brief Creates a wrapper for a string view.
	 *
	 * @param value The string view.
	 */
	explicit FStringOrStringView(FStringView value);

	/**
	 * @brief Gets the underlying string data as a string.
	 *
	 * @return The underlying string data as a string.
	 */
	[[nodiscard]] FString AsString() const;

	/**
	 * @brief Gets the underlying string data as a string view.
	 *
	 * @return The underlying string data as a string view.
	 */
	[[nodiscard]] FStringView AsStringView() const;

	/**
	 * @brief Gets the underlying string data as a character span.
	 *
	 * @return The underlying string data as a character span.
	 */
	[[nodiscard]] SpanType AsSpan() const;

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
	 * @brief Gets this string reference as a string view.
	 *
	 * @return This string reference as a string view.
	 */
	[[nodiscard]] explicit operator FStringView() const
	{
		return AsStringView();
	}

	/**
	 * @brief Gets this string reference as a character span.
	 *
	 * @return This string reference as a character span.
	 */
	[[nodiscard]] explicit operator SpanType() const
	{
		return AsSpan();
	}

private:

	VariantType m_Value;
};