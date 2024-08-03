#include "Engine/Logging.h"
#include "Object/Object.h"
#include "Object/ObjectHeap.h"
#include "Object/ObjectPtr.h"
#include "MultipleObjectClasses.h"
#include <gtest/gtest.h>

TEST(ObjectTests, DefaultConstructObjectPtr)
{
	TObjectPtr<UObject> object;
	EXPECT_FALSE(object.IsValid());
	EXPECT_TRUE(object.IsNull());
	EXPECT_EQ(object.GetObject(), nullptr);
}

TEST(ObjectTests, MakeObject)
{
	TObjectPtr<UObject> object = MakeObject<UObject>();
	EXPECT_FALSE(object.IsValid());
	EXPECT_TRUE(object.IsNull());
	EXPECT_EQ(object.GetObject(), nullptr);

	object = MakeObject<UAttributeTestClass>();
	EXPECT_TRUE(object.IsValid());
	EXPECT_FALSE(object.IsNull());
	EXPECT_NE(object.GetObject(), nullptr);
	EXPECT_TRUE(object->IsA<UObject>());
}

TEST(ObjectTests, ImplicitConvertObjectPtr)
{
	TObjectPtr<UEngine> gameEngine = MakeObject<UEditorEngine>();
	EXPECT_TRUE(gameEngine.IsValid());
	EXPECT_FALSE(gameEngine.IsNull());
	EXPECT_NE(gameEngine.GetObject(), nullptr);
}

TEST(ObjectTests, Is)
{
	TObjectPtr<UFirstDerivedTestClass> firstDerived = MakeObject<UFirstDerivedTestClass>();
	EXPECT_TRUE(firstDerived.IsValid());
	EXPECT_FALSE(firstDerived.IsNull());
	EXPECT_TRUE(firstDerived->IsA<UFirstDerivedTestClass>());
	EXPECT_FALSE(firstDerived->IsA<USecondDerivedTestClass>());
	EXPECT_TRUE(firstDerived->IsA<UBaseTestClass>());
	EXPECT_TRUE(firstDerived->IsA<UObject>());

	TObjectPtr<USecondDerivedTestClass> secondDerived = MakeObject<USecondDerivedTestClass>();
	EXPECT_TRUE(secondDerived.IsValid());
	EXPECT_FALSE(secondDerived.IsNull());
	EXPECT_FALSE(secondDerived->IsA<UFirstDerivedTestClass>());
	EXPECT_TRUE(secondDerived->IsA<USecondDerivedTestClass>());
	EXPECT_TRUE(secondDerived->IsA<UBaseTestClass>());
	EXPECT_TRUE(secondDerived->IsA<UObject>());
}

TEST(ObjectTests, ObjectCreationContext)
{
	FObjectCreationContext params;
	EXPECT_TRUE(params.SetParameter<FStringView>("title"_sv, FModuleManager::GetCurrentModuleName()));
	EXPECT_TRUE(params.SetParameter<EGraphicsApi>("graphicsApi"_sv, EGraphicsApi::OpenGL));
	EXPECT_TRUE(params.SetParameter<int32>("windowWidth"_sv, 1280));
	EXPECT_TRUE(params.SetParameter<int32>("windowHeight"_sv, 720));

	for (auto iter = params.DebugGetParameters().CreateConstIterator(); iter; ++iter)
	{
		EXPECT_FALSE(iter->Key.IsEmpty());
		EXPECT_TRUE(iter->Value.HasValue());
	}
}