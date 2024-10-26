#pragma once

#include "Object/Object.h"
#include "MultipleObjectClasses.Generated.h"

UM_CLASS(Abstract)
class UBaseTestClass : public UObject
{
	UM_GENERATED_BODY();
};

UM_CLASS()
class UFirstDerivedTestClass : public UBaseTestClass
{
	UM_GENERATED_BODY();
};

UM_CLASS()
class USecondDerivedTestClass : public UBaseTestClass
{
	UM_GENERATED_BODY();
};

UM_CLASS(Test, Foo = Bar, MinValue=42)
class UAttributeTestClass : public UObject
{
	UM_GENERATED_BODY();
};

UM_CLASS()
class UChildClassContainer : public UObject
{
	UM_GENERATED_BODY();
};

UM_CLASS(ChildOf=UChildClassContainer)
class UChildClass : public UObject
{
	UM_GENERATED_BODY();

public:

	FStringView GetParentName() const;
};