#include "Misc/SourceLocation.h"
#include "Containers/InternalString.h"

void TFormatter<FCppSourceLocation>::BuildString(const FCppSourceLocation& value, FStringBuilder& builder) const
{
	builder.Append(value.GetSourceName());
	builder.Append(":"_sv);
	builder.Append(value.GetSourceLine());
}

bool TFormatter<FCppSourceLocation>::Parse(FStringView formatString)
{
	return formatString.IsEmpty();
}