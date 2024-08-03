#pragma once

#include "Containers/String.h"
#include "HAL/FileStream.h"
#include "Memory/WeakPtr.h"

/**
 * @brief Defines a helper for writing formatted string content to a stream.
 */
class FTextStreamWriter
{
public:

	/**
	 * @brief Resets the indent level to zero.
	 */
	void ClearIndent();

	/**
	 * @brief Gets this writer's current indent level.
	 *
	 * @return This writer's indent level.
	 */
	[[nodiscard]] int32 GetIndentLevel() const;

	/**
	 * @brief Increases this writer's indent level by one.
	 */
	void Indent();

	/**
	 * @brief Sets the file stream to be used by this string writer.
	 *
	 * @param fileStream The underlying file stream.
	 */
	void SetFileStream(TSharedPtr<IFileStream> fileStream);

	/**
	 * @brief Decreases this writer's indent level by one if it is greater than zero.
	 */
	void Unindent();

	/**
	 * @brief Writes the given string to the stream.
	 * .
	 * @param string The string to write.
	 */
	void Write(FStringView string);

	/**
	 * @brief Writes the given string, without indents, to the stream.
	 * .
	 * @param string The string to write.
	 */
	void WriteNoIndent(FStringView string);

	/**
	 * @brief Writes a formatted string to this stream.
	 *
	 * @tparam ArgTypes The argument types.
	 * @param formatString The format string.
	 * @param args The format arguments.
	 */
	template<typename... ArgTypes>
	void Write(FStringView formatString, ArgTypes&&... args)
	{
		const FString formattedString = FString::Format(formatString, Forward<ArgTypes>(args)...);
		Write(formattedString);
	}

	/**
	 * @brief Writes the given string as well as a line separator to this stream.
	 *
	 * @param string The string to write.
	 */
	void WriteLine(FStringView string = ""_sv);

	/**
	 * @brief Writes the given string without indents as well as a line separator to this stream.
	 *
	 * @param string The string to write.
	 */
	void WriteLineNoIndent(FStringView string);

	/**
	 * @brief Writes a formatted string as well as a line separator to this stream.
	 *
	 * @tparam ArgTypes The argument types.
	 * @param formatString The format string.
	 * @param args The format arguments.
	 */
	template<typename... ArgTypes>
	void WriteLine(FStringView formatString, ArgTypes&&... args)
	{
		const FString formattedString = FString::Format(formatString, Forward<ArgTypes>(args)...);
		WriteLine(formattedString);
	}

	/**
	 * @brief Writes a formatted string without indents as well as a line separator to this stream.
	 *
	 * @tparam ArgTypes The argument types.
	 * @param formatString The format string.
	 * @param args The format arguments.
	 */
	template<typename... ArgTypes>
	void WriteLineNoIndent(FStringView formatString, ArgTypes&&... args)
	{
		const FString formattedString = FString::Format(formatString, Forward<ArgTypes>(args)...);
		WriteLineNoIndent(formattedString);
	}

private:

	/**
	 * @brief Gets a value indicating whether or not this string writer can write to its referenced stream.
	 *
	 * @return True if this string writer can write to the referenced stream, otherwise false.
	 */
	[[nodiscard]] bool CanWriteToStream() const;

	TWeakPtr<IFileStream> m_FileStream;
	int32 m_IndentLevel = 0;
};