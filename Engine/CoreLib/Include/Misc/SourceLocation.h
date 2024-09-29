#pragma once

#include "Containers/StringView.h"
#include "Engine/MiscMacros.h"
#include "Misc/StringFormatting.h"

/**
 * @brief Defines a C++ source location.
 */
class FCppSourceLocation
{
public:

	/**
	 * @brief Sets default values for this source location's properties.
	 */
	constexpr FCppSourceLocation() = default;

	/**
	 * @brief Sets default values for this source location's properties.
	 *
	 * @param sourceName The source name.
	 * @param sourceLine The source line.
	 */
	constexpr FCppSourceLocation(FStringView sourceName, int64 sourceLine)
		: m_SourceName { MoveTemp(sourceName) }
		, m_SourceLine { sourceLine }
	{
	}

	/**
	 * @brief Gets the line in the source.
	 *
	 * @return The line in the source.
	 */
	[[nodiscard]] constexpr int64 GetSourceLine() const
	{
		return m_SourceLine;
	}

	/**
	 * @brief Gets the name of the source.
	 *
	 * @return The name of the source.
	 */
	[[nodiscard]] constexpr FStringView GetSourceName() const
	{
		return m_SourceName;
	}

private:

	FStringView m_SourceName;
	int64 m_SourceLine = 0;
};

template<>
struct TFormatter<FCppSourceLocation>
{
	void BuildString(const FCppSourceLocation& value, FStringBuilder& builder) const;
	bool Parse(FStringView formatString);
};

/**
 * @brief Creates a FSourceLocation for the current line.
 */
#define UMBRAL_SOURCE_LOCATION FCppSourceLocation(UMBRAL_FILE_AS_VIEW, UMBRAL_LINE)