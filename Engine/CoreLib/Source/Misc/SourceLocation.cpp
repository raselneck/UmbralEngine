#include "Misc/SourceLocation.h"
#include "Containers/InternalString.h"

void TFormatter<FSourceLocation>::BuildString(const FSourceLocation& value, FStringBuilder& builder) const
{
	builder.Append(value.GetSourceName());
	builder.Append(":"_sv);
	builder.Append(value.GetSourceLine());
}

bool TFormatter<FSourceLocation>::Parse(FStringView formatString)
{
	return formatString.IsEmpty();
}