#include "Containers/String.h"
#include "Misc/StringBuilder.h"
#include "Misc/Version.h"

FString FVersion::AsString() const
{
	return AsString(EVersionStringFlags::None);
}

FString FVersion::AsString(const EVersionStringFlags flags) const
{
	FStringBuilder builder = FStringBuilder{}
		.Reserve(24)
		.Append(m_Major)
		.Append("."_sv)
		.Append(m_Minor);

	const bool includeBuild = m_Build > 0 || HasFlag(flags, EVersionStringFlags::IncludeBuild);
	const bool includePatch = m_Patch > 0 || HasFlag(flags, EVersionStringFlags::IncludePatch) || includeBuild;

	if (includePatch)
	{
		builder.Append("."_sv).Append(m_Patch);
	}

	if (includeBuild)
	{
		if (HasFlag(flags, EVersionStringFlags::HyphenateBuildAsHex))
		{
			builder.Append("-"_sv).Append(m_Build, ENumericBase::Hexadecimal);
		}
		else
		{
			builder.Append("."_sv).Append(m_Build);
		}
	}

	return builder.ReleaseString();
}