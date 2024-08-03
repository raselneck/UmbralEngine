#include "Engine/Logging.h"
#include "Engine/MiscMacros.h"
#include "Meta/TypeInfo.h"
#include "Misc/StringBuilder.h"
#include "MultipleObjectClasses.h"
#include <gtest/gtest.h>

[[maybe_unused]] static FString GetStringRepresentationOfClassInfo(const FClassInfo* classInfo)
{
	FStringBuilder builder;
	builder.Reserve(512);

	builder.Append("Class Name:  {}\n"_sv, classInfo->GetName());
	builder.Append("Size:        {}\n"_sv, classInfo->GetSize());
	builder.Append("Alignment:   {}\n"_sv, classInfo->GetAlignment());

	builder.Append("Attributes:  {}\n"_sv, classInfo->GetNumAttributes());
	for (int32 idx = 0; idx < classInfo->GetNumAttributes(); ++idx)
	{
		const FAttributeInfo* attribute = classInfo->GetAttribute(idx);
		if (attribute->HasValue())
		{
			builder.Append("\t\"{}\" = \"{}\"\n"_sv, attribute->GetName(), attribute->GetValue());
		}
		else
		{
			builder.Append("\t\"{}\"\n"_sv, attribute->GetName());
		}
	}

	builder.Append("Properties:  {}\n"_sv, classInfo->GetNumProperties());
	for (int32 idx = 0; idx < classInfo->GetNumProperties(); ++idx)
	{
		const FPropertyInfo* property = classInfo->GetProperty(idx);
		builder.Append("\t\"{}\"\n"_sv, property->GetName());
		builder.Append("\t\tOffset:    {}\n"_sv, property->GetOffset());
		builder.Append("\t\tType:      \"{}\"\n"_sv, property->GetValueType()->GetName());
		builder.Append("\t\tAttributes: {}\n"_sv, property->GetNumAttributes());
	}

	return builder.ReleaseString();
}

TEST(MetaTests, Attributes)
{
	const FClassInfo* classInfo = GetType<UAttributeTestClass>();
	EXPECT_NE(classInfo, nullptr);

	EXPECT_EQ(classInfo->GetNumAttributes(), 3);

	const FAttributeInfo* attrib1 = classInfo->GetAttribute(0);
	const FAttributeInfo* attrib2 = classInfo->GetAttribute(1);
	const FAttributeInfo* attrib3 = classInfo->GetAttribute(2);
	const FAttributeInfo* attrib4 = classInfo->GetAttribute(3);

	EXPECT_NE(attrib1, nullptr);
	EXPECT_EQ(attrib1->GetName(), "Test"_sv);
	EXPECT_FALSE(attrib1->HasValue());

	EXPECT_NE(attrib2, nullptr);
	EXPECT_EQ(attrib2->GetName(), "Foo"_sv);
	EXPECT_TRUE(attrib2->HasValue());
	EXPECT_EQ(attrib2->GetValue(), "Bar"_sv);

	EXPECT_NE(attrib3, nullptr);
	EXPECT_EQ(attrib3->GetName(), "MinValue"_sv);
	EXPECT_TRUE(attrib3->HasValue());
	EXPECT_EQ(attrib3->GetValue(), "42"_sv);

	EXPECT_EQ(attrib4, nullptr);
}

TEST(MetaTests, Properties)
{
	const FClassInfo* classInfo = GetType<UAttributeTestClass>();
	EXPECT_NE(classInfo, nullptr);

	EXPECT_GT(classInfo->GetNumProperties(), 0);

	const FPropertyInfo* parentProperty = classInfo->GetPropertyByName("m_Parent"_sv);
	EXPECT_NE(parentProperty, nullptr);
	EXPECT_EQ(parentProperty->GetName(), "m_Parent"_sv);
	EXPECT_EQ(parentProperty->GetValueType(), GetType<FObjectPtr>());
	EXPECT_EQ(parentProperty->GetOffset(), UObject::GetOffsetOfParentProperty<UObject>());
	EXPECT_EQ(parentProperty->GetOffset(), UObject::GetOffsetOfParentProperty<UAttributeTestClass>());

	const FPropertyInfo* nameProperty = classInfo->GetPropertyByName("m_Name"_sv);
	EXPECT_NE(nameProperty, nullptr);
	EXPECT_EQ(nameProperty->GetName(), "m_Name"_sv);
	EXPECT_EQ(nameProperty->GetValueType(), GetType<FString>());
	EXPECT_EQ(nameProperty->GetOffset(), UObject::GetOffsetOfNameProperty<UObject>());
	EXPECT_EQ(nameProperty->GetOffset(), UObject::GetOffsetOfNameProperty<UAttributeTestClass>());

	TObjectPtr<UAttributeTestClass> objectValue = MakeObject<UAttributeTestClass>(nullptr, "TestName"_sv);
	EXPECT_TRUE(objectValue.IsValid());

	const FObjectPtr* parentPropertyValue = parentProperty->GetValue<FObjectPtr>(*objectValue);
	EXPECT_NE(parentPropertyValue, nullptr);
	EXPECT_EQ(parentPropertyValue, objectValue->GetPointerToParentProperty());

	const FString* namePropertyValue = nameProperty->GetValue<FString>(*objectValue);
	EXPECT_NE(namePropertyValue, nullptr);
	EXPECT_EQ(namePropertyValue, objectValue->GetPointerToNameProperty());
	EXPECT_TRUE(namePropertyValue->StartsWith("TestName"_sv));
}

TEST(MetaTests, PrimitiveTypes)
{
	const FTypeInfo* typeInfo = GetType<int8>();
	EXPECT_TRUE(typeInfo != nullptr);
	EXPECT_EQ(typeInfo->GetAlignment(), alignof(int8));
	EXPECT_EQ(typeInfo->GetName(), "int8"_sv);
	EXPECT_EQ(typeInfo->GetSize(), sizeof(int8));

	typeInfo = GetType<float>();
	EXPECT_TRUE(typeInfo != nullptr);
	EXPECT_EQ(typeInfo->GetAlignment(), alignof(float));
	EXPECT_EQ(typeInfo->GetName(), "float"_sv);
	EXPECT_EQ(typeInfo->GetSize(), sizeof(float));

	typeInfo = GetType<char16_t>();
	EXPECT_TRUE(typeInfo != nullptr);
	EXPECT_EQ(typeInfo->GetAlignment(), alignof(char16_t));
	EXPECT_EQ(typeInfo->GetName(), "char16_t"_sv);
	EXPECT_EQ(typeInfo->GetSize(), sizeof(char16_t));

	EXPECT_NE(GetType<int8>(), GetType<char>());
	EXPECT_NE(GetType<uint8>(), GetType<char>());
	EXPECT_EQ(GetType<char>(), GetType<char>());
	EXPECT_EQ(GetType<int>(), GetType<int32>());
	EXPECT_EQ(GetType<unsigned>(), GetType<uint32>());
}

TEST(MetaTests, ChildOfAttribute)
{
	TObjectPtr<UChildClass> childClass = MakeObject<UChildClass>();
	EXPECT_TRUE(childClass.IsNull());
	EXPECT_FALSE(childClass.IsValid());

	TObjectPtr<UChildClassContainer> childContainer = MakeObject<UChildClassContainer>();
	EXPECT_TRUE(childContainer.IsValid());
	EXPECT_FALSE(childContainer.IsNull());

	childClass = MakeObject<UChildClass>(childContainer);
	EXPECT_TRUE(childClass.IsValid());
	EXPECT_FALSE(childClass.IsNull());
}

TEST(MetaTests, Parent)
{
	TObjectPtr<UChildClassContainer> childContainer = MakeObject<UChildClassContainer>();
	EXPECT_TRUE(childContainer.IsValid());
	EXPECT_FALSE(childContainer.IsNull());

	TObjectPtr<UChildClass> childClass = MakeObject<UChildClass>(childContainer);
	EXPECT_TRUE(childClass.IsValid());
	EXPECT_FALSE(childClass.IsNull());

	const FStringView childContainerName = childClass->GetParentName();
	EXPECT_TRUE(childContainerName.StartsWith(nameof(UChildClassContainer)));
}