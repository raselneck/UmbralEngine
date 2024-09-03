#pragma once

#include "Containers/String.h"
#include "Containers/StringView.h"

/**
 * @brief An enumeration of console colors.
 */
enum class EConsoleColor : uint8
{
	Default,
	Black,
	Red,
	Green,
	Yellow,
	Blue,
	Magenta,
	Cyan,
	White,
	BrightBlack,
	BrightRed,
	BrightGreen,
	BrightYellow,
	BrightBlue,
	BrightMagenta,
	BrightCyan,
	BrightWhite
};

/**
 * @brief An set of text style flags for the console.
 */
enum class EConsoleTextStyle : uint8
{
	None = 0,
	Bold = (1 << 0),
	Underline = (1 << 1)
};

ENUM_FLAG_OPERATORS(EConsoleTextStyle);

/**
 * @brief Defines a platform-independent way of interacting with the console.
 */
class FConsole
{
public:

	/**
	 * @brief Flushes the console.
	 */
	static void Flush();

	/**
	 * @brief Gets the console's background color.
	 *
	 * @return The console's background color.
	 */
	[[nodiscard]] static EConsoleColor GetBackgroundColor();

	/**
	 * @brief Gets the console's background color.
	 *
	 * @return The console's background color.
	 */
	[[nodiscard]] static EConsoleColor GetForegroundColor();

	/**
	 * @brief Gets the console's text style.
	 *
	 * @return The text style.
	 */
	[[nodiscard]] static EConsoleTextStyle GetTextStyle();

	/**
	 * @brief Reads a single character from the console.
	 *
	 * @return The read character.
	 */
	[[maybe_unused]] static FString::CharType ReadChar();

	/**
	 * @brief Reads a line of text entered into the console.
	 *
	 * @return The read string.
	 */
	[[nodiscard]] static FString ReadLine();

	/**
	 * @brief Resets all console color and text style attributes.
	 */
	static void ResetAttributes();

	/**
	 * @brief Sets the console's background color.
	 *
	 * @param color The new background color.
	 */
	static void SetBackgroundColor(EConsoleColor color);

	/**
	 * @brief Sets the console's background color.
	 *
	 * @param color The new foreground color.
	 */
	static void SetForegroundColor(EConsoleColor color);

	/**
	 * @brief Sets the console's text style.
	 *
	 * @param style The new text style.
	 */
	static void SetTextStyle(EConsoleTextStyle style);

	/**
	 * @brief Writes a formatted string to the standard output.
	 *
	 * @tparam ArgTypes The types of the format arguments.
	 * @param message The message format.
	 * @param args The format arguments.
	 */
	template<typename... ArgTypes>
	static void Write(const FStringView message, ArgTypes&&... args)
	{
		const FString text = FString::Format(message, Forward<ArgTypes>(args)...);
		Write(text.AsStringView());
	}

	/**
	 * @brief Writes the given text to the console.
	 *
	 * @param text The text.
	 */
	static void Write(FStringView text);

	/**
	 * @brief Writes a formatted string, followed by a new line, to the given console target.
	 *
	 * @tparam ArgTypes The types of the format arguments.
	 * @param message The message format.
	 * @param args The format arguments.
	 */
	template<typename... ArgTypes>
	static void WriteLine(const FStringView message, ArgTypes&&... args)
	{
		const FString text = FString::Format(message, Forward<ArgTypes>(args)...);
		WriteLine(text.AsStringView());
	}

	/**
	 * @brief Writes the given text, followed by a new line, to the console.
	 *
	 * @param text The text.
	 */
	static void WriteLine(FStringView text);

	/**
	 * @brief Writes a new line to the console.
	 */
	static void WriteLine();
};

/**
 * @brief Helps to set the console background color for the duration of a scope.
 */
class FScopedConsoleBackgroundColor final
{
public:

	/**
	 * @brief Caches the current console background color and then sets it to \p newColor.
	 *
	 * @param newColor The new background color.
	 */
	explicit FScopedConsoleBackgroundColor(EConsoleColor newColor);

	/**
	 * @brief Restores the console's foreground color.
	 */
	~FScopedConsoleBackgroundColor();

private:

	const EConsoleColor m_BackgroundColor;
};

/**
 * @brief Helps to set the console foreground color for the duration of a scope.
 */
class FScopedConsoleForegroundColor final
{
public:

	/**
	 * @brief Caches the current console foreground color and then sets it to \p newColor.
	 *
	 * @param newColor The new foreground color.
	 */
	explicit FScopedConsoleForegroundColor(EConsoleColor newColor);

	/**
	 * @brief Restores the console's foreground color.
	 */
	~FScopedConsoleForegroundColor();

private:

	const EConsoleColor m_ForegroundColor;
};