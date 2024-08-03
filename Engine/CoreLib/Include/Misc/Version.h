#pragma once

#include "Engine/IntTypes.h"
#include "Templates/ComparisonTraits.h"

class FString;

/**
 * @brief A set of flags that can be used to configure how a FVersion can be built as a string.
 */
enum class EVersionStringFlags
{
	None = 0,

	/**
	 * @brief The patch version number will be included even if it is zero.
	 */
	IncludePatch = (1 << 0),

	/**
	 * @brief The build version number will be included even if it is zero. If BuildAsHex is also specified,
	 *        then the patch number can be excluded. If not, then this implies IncludePatch.
	 */
	IncludeBuild = (1 << 1),

	/**
	 * @brief If the build version number if included, then it will be added as `-&lt;build as hex&gt;`
	 *        instead of `.&lt;build as decimal&gt;`.
	 */
	HyphenateBuildAsHex = (1 << 2)
};

ENUM_FLAG_OPERATORS(EVersionStringFlags)

/**
 * @brief Defines a version.
 */
class FVersion
{
public:

	/**
	 * @brief Sets default values for this version's properties.
	 */
	FVersion() = default;

	/**
	 * @brief Sets default values for this version's properties.
	 *
	 * @param major The major version number.
	 * @param minor The minor version number.
	 */
	constexpr FVersion(const uint32 major, const uint32 minor) noexcept
		: FVersion(major, minor, 0, 0)
	{
	}

	/**
	 * @brief Sets default values for this version's properties.
	 *
	 * @param major The major version number.
	 * @param minor The minor version number.
	 * @param patch The patch version number.
	 */
	constexpr FVersion(const uint32 major, const uint32 minor, const uint32 patch) noexcept
		: FVersion(major, minor, patch, 0)
	{
	}

	/**
	 * @brief Sets default values for this version's properties.
	 *
	 * @param major The major version number.
	 * @param minor The minor version number.
	 * @param patch The patch version number.
	 * @param build The build version number.
	 */
	constexpr FVersion(const uint32 major, const uint32 minor, const uint32 patch, const uint32 build) noexcept
		: m_Major { major }
		, m_Minor { minor }
		, m_Patch { patch }
		, m_Build { build }
	{
	}

	/**
	 * @brief Gets this version as a string.
	 *
	 * @return A string representing this version.
	 */
	[[nodiscard]] FString AsString() const;

	/**
	 * @brief Gets this version as a string.
	 *
	 * @param flags The flags to use when building the version string.
	 * @return A string representing this version.
	 */
	[[nodiscard]] FString AsString(EVersionStringFlags flags) const;

	/**
	 * @brief Compares this version to another version.
	 *
	 * @param other The other version.
	 * @return The comparison result.
	 */
	[[nodiscard]] constexpr ECompareResult Compare(const FVersion& other) const
	{
		if (m_Major < other.GetMajor())
		{
			return ECompareResult::LessThan;
		}
		if (m_Major > other.GetMajor())
		{
			return ECompareResult::GreaterThan;
		}

		if (m_Minor < other.GetMinor())
		{
			return ECompareResult::LessThan;
		}
		if (m_Minor > other.GetMinor())
		{
			return ECompareResult::GreaterThan;
		}

		if (m_Patch < other.GetPatch())
		{
			return ECompareResult::LessThan;
		}
		if (m_Patch > other.GetPatch())
		{
			return ECompareResult::GreaterThan;
		}

		if (m_Build < other.GetBuild())
		{
			return ECompareResult::LessThan;
		}
		if (m_Build > other.GetBuild())
		{
			return ECompareResult::GreaterThan;
		}

		return ECompareResult::Equals;
	}

	/**
	 * @brief Gets this version's build number.
	 *
	 * @return This version's build number.
	 */
	[[nodiscard]] constexpr uint32 GetBuild() const
	{
		return m_Build;
	}

	/**
	 * @brief Gets this version's major number.
	 *
	 * @return This version's major number.
	 */
	[[nodiscard]] constexpr uint32 GetMajor() const
	{
		return m_Major;
	}

	/**
	 * @brief Gets this version's minor number.
	 *
	 * @return This version's minor number.
	 */
	[[nodiscard]] constexpr uint32 GetMinor() const
	{
		return m_Minor;
	}

	/**
	 * @brief Gets this version's patch number.
	 *
	 * @return This version's patch number.
	 */
	[[nodiscard]] constexpr uint32 GetPatch() const
	{
		return m_Patch;
	}

	// TODO static FVersion FromString(const FString& value)
	// TODO static FVersion FromString(const FStringView value)

	constexpr bool operator< (const FVersion& other) const { return Compare(other) == ECompareResult::LessThan; }
	constexpr bool operator<=(const FVersion& other) const { return Compare(other) != ECompareResult::GreaterThan; }
	constexpr bool operator> (const FVersion& other) const { return Compare(other) == ECompareResult::GreaterThan; }
	constexpr bool operator>=(const FVersion& other) const { return Compare(other) != ECompareResult::LessThan; }
	constexpr bool operator==(const FVersion& other) const { return Compare(other) == ECompareResult::Equals; }
	constexpr bool operator!=(const FVersion& other) const { return Compare(other) != ECompareResult::Equals; }

private:

	uint32 m_Major = 0;
	uint32 m_Minor = 0;
	uint32 m_Patch = 0;
	uint32 m_Build = 0;
};