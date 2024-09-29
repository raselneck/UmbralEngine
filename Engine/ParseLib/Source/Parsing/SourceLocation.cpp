#include "Parsing/SourceLocation.h"
#include "Misc/StringBuilder.h"

void TFormatter<FSourceLocation>::BuildString(const FSourceLocation& value, FStringBuilder& builder) const
{
	builder.Append("({}:{})"_sv, value.Line, value.Column);
}

bool TFormatter<FSourceLocation>::Parse(const FStringView formatString)
{
	(void)formatString;
	return true;
}