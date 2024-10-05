#include "JSON/JsonScanner.h"

FJsonScanner::FJsonScanner()
{
	SetLineCommentBegin("//"_sv);
	SetMultiLineComment("/*"_sv, "*/"_sv);
	SetShouldRecordComments(false);
}

bool FJsonScanner::TryScanTokenFromCurrentPosition()
{
	return false;
}
