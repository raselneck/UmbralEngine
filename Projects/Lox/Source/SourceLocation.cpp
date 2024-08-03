#include "Lox/SourceLocation.h"
#include "Misc/StringBuilder.h"

void TFormatter<FLoxSourceLocation>::BuildString(const FLoxSourceLocation& value, FStringBuilder& builder)
{
	builder.Append("({}:{})"_sv, value.Line, value.Column);
}

bool TFormatter<FLoxSourceLocation>::Parse(FStringView formatString)
{
	(void)formatString;
	return true;
}