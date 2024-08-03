#include "MetaTool/ClassInfo.h"
#include "MetaTool/PropertyInfo.h"

bool FParsedPropertyInfo::IsObjectProperty() const
{
	return FParsedClassInfo::IsObjectBasedName(PropertyType);
}