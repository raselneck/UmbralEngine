#include "MultipleObjectClasses.h"
#include "Object/ObjectPtr.h"

FStringView UChildClass::GetParentName() const
{
	TObjectPtr<UChildClassContainer> parent = GetTypedParent<UChildClassContainer>();
	return parent->GetName();
}