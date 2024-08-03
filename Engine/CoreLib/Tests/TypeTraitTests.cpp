#include "Containers/Function.h"
#include "Containers/StaticArray.h"
#include "Templates/TypeTraits.h"
#include <gtest/gtest.h>

template<typename TypeToDecay, typename ExpectedDecayedType>
using TIsDecayedType = TIsSame<typename TDecay<TypeToDecay>::Type, ExpectedDecayedType>;

TEST(TypeTraitTests, Decay)
{
	// https://en.cppreference.com/w/cpp/types/decay

	EXPECT_TRUE((TIsDecayedType<int, int>::Value));
	EXPECT_FALSE((TIsDecayedType<int, float>::Value));
	EXPECT_TRUE((TIsDecayedType<int&, int>::Value));
	EXPECT_TRUE((TIsDecayedType<int&&, int>::Value));
	EXPECT_TRUE((TIsDecayedType<const int&, int>::Value));
	EXPECT_TRUE((TIsDecayedType<int[2], int*>::Value));
	EXPECT_FALSE((TIsDecayedType<int[4][2], int*>::Value));
	EXPECT_FALSE((TIsDecayedType<int[4][2], int**>::Value));
	EXPECT_TRUE((TIsDecayedType<int[4][2], int(*)[2]>::Value));
	EXPECT_TRUE((TIsDecayedType<int(int), int(*)(int)>::Value));
}

TEST(TypeTraitTests, Declval)
{
	struct Foo
	{
		Foo() = delete;
		int Bar() const;
	};

	EXPECT_TRUE((TIsSame<int, decltype(declval<Foo>().Bar())>::Value));
}

TEST(TypeTraitTests, IsAbstract)
{
	struct AbstractBase
	{
		virtual void Foo() const = 0;
	};

	struct ConcreteDerived : AbstractBase
	{
		void Foo() const override { }
	};

	EXPECT_TRUE(TIsAbstract<AbstractBase>::Value);
	EXPECT_FALSE(TIsAbstract<ConcreteDerived>::Value);
}

TEST(TypeTraitTests, IsArray)
{
	// https://en.cppreference.com/w/cpp/types/is_array

	struct A {};

	EXPECT_FALSE(TIsArray<A>::Value);
	EXPECT_TRUE(TIsArray<A[]>::Value);
	EXPECT_TRUE(TIsArray<A[3]>::Value);
	EXPECT_FALSE(TIsArray<float>::Value);
	EXPECT_FALSE(TIsArray<int>::Value);
	EXPECT_TRUE(TIsArray<int[]>::Value);
	EXPECT_TRUE(TIsArray<int[3]>::Value);
	EXPECT_FALSE((TIsArray<TStaticArray<int, 3>>::Value));
	EXPECT_TRUE((TIsArray<decltype(declval<TStaticArray<int, 3>>().m_Data)>::Value));
}

TEST(TypeTraitTests, ArrayRank)
{
	EXPECT_EQ(TArrayRank<int>::Value, 0);
	EXPECT_EQ(TArrayRank<int[]>::Value, 1);
	EXPECT_EQ(TArrayRank<int[5]>::Value, 1);
	EXPECT_EQ(TArrayRank<int[][5]>::Value, 2);
}

TEST(TypeTraitTests, ArrayExtent)
{
	[[maybe_unused]] constexpr int ints[]  = { 1, 2, 3, 4 };
	[[maybe_unused]] constexpr int array[][3] ={{ 1, 2, 3 }};

	static_assert(TIsSame<decltype(array[0]), const int(&)[3]>::Value);

	EXPECT_EQ(TArrayExtent<int[3]>::Value, 3);
	EXPECT_EQ((TArrayExtent<int[3][4], 0>::Value), 3);
	EXPECT_EQ((TArrayExtent<int[3][4], 1>::Value), 4);
	EXPECT_EQ((TArrayExtent<int[3][4], 2>::Value), 0);
	EXPECT_EQ(TArrayExtent<int[]>::Value, 0);
	EXPECT_EQ(TArrayExtent<int[9]>::Value, 9);
	EXPECT_EQ(TArrayExtent<decltype(ints)>::Value, 4);

	// NOTE The following is a "bug" or "feature" with std::extent as well
	// `array[0]'' is type of reference of `int[3]', so TArrayExtent cannot calculate correctly and return 0
	EXPECT_EQ(TArrayExtent<decltype(array[0])>::Value, 0);
	EXPECT_EQ(TArrayExtent<typename TRemoveConstVolatileReference<decltype(array[0])>::Type>::Value, 3);
}

TEST(TypeTraitTests, RemoveExtent)
{
	// TODO What do we do here?
}

TEST(TypeTraitTests, IsAssignable)
{
	enum NonClassEnum { };

	EXPECT_FALSE((TIsAssignable<int, int>::Value)); // 1 = 1; is not valid
	EXPECT_TRUE((TIsAssignable<int&, int>::Value)); // int a; a = 1; is valid
	EXPECT_FALSE((TIsAssignable<NonClassEnum&, int>::Value));
	EXPECT_TRUE((TIsAssignable<int&, NonClassEnum>::Value));
}

TEST(TypeTraitTests, IsTriviallyAssignable)
{
	struct Foo { };

	EXPECT_FALSE((TIsTriviallyAssignable<Foo, int>::Value));
	EXPECT_TRUE((TIsTriviallyAssignable<Foo&, const Foo&>::Value));
	EXPECT_TRUE((TIsTriviallyAssignable<Foo&, Foo&&>::Value));
	EXPECT_TRUE(TIsTriviallyCopyAssignable<Foo>::Value);
	EXPECT_TRUE(TIsTriviallyMoveAssignable<Foo>::Value);
}

TEST(TypeTraitTests, IsBaseOf)
{
	struct A {};
	struct B : A {};
	struct C : protected A {};
	struct D : B {};
	struct E {};
	struct F : private E {};

	// Base type on left, derived type on right
	EXPECT_TRUE((TIsBaseOf<A, B>::Value));
	EXPECT_FALSE((TIsBaseOf<B, A>::Value));
	EXPECT_TRUE((TIsBaseOf<A, C>::Value));
	EXPECT_FALSE((TIsBaseOf<C, A>::Value));
	EXPECT_TRUE((TIsBaseOf<A, D>::Value));
	EXPECT_FALSE((TIsBaseOf<D, A>::Value));
	EXPECT_TRUE((TIsBaseOf<B, D>::Value));
	EXPECT_FALSE((TIsBaseOf<D, B>::Value));
	EXPECT_FALSE((TIsBaseOf<A, E>::Value));
	EXPECT_FALSE((TIsBaseOf<E, A>::Value));
	EXPECT_FALSE((TIsBaseOf<A, int>::Value));
	EXPECT_FALSE((TIsBaseOf<int, A>::Value));
	EXPECT_TRUE((TIsBaseOf<E, F>::Value));
	EXPECT_FALSE((TIsBaseOf<F, E>::Value));
}

TEST(TypeTraitTests, IsClass)
{
	enum NonClassEnum { };
	enum class ClassEnum { };
	struct Foo { };
	class Bar { };
	union Baz { Foo F; Bar B; };

	EXPECT_FALSE(TIsClass<NonClassEnum>::Value);
	EXPECT_FALSE(TIsClass<ClassEnum>::Value);
	EXPECT_TRUE(TIsClass<Foo>::Value);
	EXPECT_TRUE(TIsClass<Bar>::Value);
	EXPECT_FALSE(TIsClass<Baz>::Value);
}

TEST(TypeTraitTests, IsCallable)
{
	EXPECT_TRUE((TIsCallable<void, void(*)()>::Value));
	EXPECT_FALSE((TIsCallable<int32, void(*)(int32), int32>::Value));

	const auto NoParamsReturnInt = [] { return 42; };
	EXPECT_TRUE((TIsCallable<int32, decltype(NoParamsReturnInt)>::Value));

	struct FIntParamNoReturn { void operator()(const int32) const {} };
	EXPECT_TRUE((TIsCallable<void, FIntParamNoReturn, int32>::Value));

	EXPECT_TRUE((TIsCallable<int32, TFunction<int32(int32)>, int32>::Value));

	struct FEmptyType { };
	EXPECT_FALSE((TIsCallable<void, FEmptyType>::Value));
	EXPECT_FALSE((TIsCallable<int32, FEmptyType>::Value));
	EXPECT_FALSE((TIsCallable<float, FEmptyType, int32>::Value));
}

// TODO IsConstructible

// TODO IsTriviallyConstructible

// TODO IsConst

// TODO IsVolatile

// TODO IsConstVolatile

// TODO IsConvertible

// TODO IsEnum

// TODO IsEnumClass

// TODO IsFloat

// TODO IsFunction

// TODO IsInt

// TODO IsSigned

// TODO IsUnsigned

// TODO IsPointer

// TODO AddPointer

// TODO RemovePointer

// TODO IsLValueReference

// TODO IsRValueReference

// TODO IsReference

// TODO AddLValueReference

// TODO AddRValueReference

// TODO RemoveReference

// TODO IsSame

TEST(TypeTraitTests, IsUnion)
{
	enum NonClassEnum { };
	enum class ClassEnum { };
	struct Foo { };
	class Bar { };
	union Baz { Foo F; Bar B; };

	EXPECT_FALSE(TIsUnion<NonClassEnum>::Value);
	EXPECT_FALSE(TIsUnion<ClassEnum>::Value);
	EXPECT_FALSE(TIsUnion<Foo>::Value);
	EXPECT_FALSE(TIsUnion<Bar>::Value);
	EXPECT_TRUE(TIsUnion<Baz>::Value);
}

TEST(TypeTraitTests, IsVoid)
{
	struct Foo { };

	EXPECT_TRUE(TIsVoid<void>::Value);
	EXPECT_FALSE(TIsVoid<int>::Value);
	EXPECT_FALSE(TIsVoid<double>::Value);
	EXPECT_FALSE(TIsVoid<Foo>::Value);
}

// TODO UnderlyingType

TEST(TypeTraitTests, RemoveConstVolatileReference)
{
	EXPECT_TRUE((TIsSame<typename TRemoveConstVolatileReference<int>::Type, int>::Value));
	EXPECT_TRUE((TIsSame<typename TRemoveConstVolatileReference<int&>::Type, int>::Value));
	EXPECT_TRUE((TIsSame<typename TRemoveConstVolatileReference<int&&>::Type, int>::Value));
	EXPECT_TRUE((TIsSame<typename TRemoveConstVolatileReference<const int&>::Type, int>::Value));
	EXPECT_TRUE((TIsSame<typename TRemoveConstVolatileReference<const int[2]>::Type, int[2]>::Value));
	EXPECT_TRUE((TIsSame<typename TRemoveConstVolatileReference<const int(&)[2]>::Type, int[2]>::Value));
	EXPECT_TRUE((TIsSame<typename TRemoveConstVolatileReference<int(int)>::Type, int(int)>::Value));
}

// TODO TIsMemberPointer

// TODO TIsMemberFunctionPointer

// TODO TIsMemberObjectPointer

// TODO TVariadicIndexOfType

TEST(TypeTraitTests, TVariadicTypeAtIndex)
{
	EXPECT_TRUE((TIsSame<int, typename TVariadicTypeAtIndex<0, int, float, char>::Type>::Value));
	EXPECT_TRUE((TIsSame<float, typename TVariadicTypeAtIndex<1, int, float, char>::Type>::Value));
	EXPECT_TRUE((TIsSame<char, typename TVariadicTypeAtIndex<2, int, float, char>::Type>::Value));

	// Compile error for this one :)
	//EXPECT_TRUE((TIsSame<void, typename TVariadicTypeAtIndex<3, int, float, char>::Type>::Value));
}

// TODO TVariadicContainsType

// TODO TVariadicContainsLValueReference

// TODO TVariadicContainsRValueReference

// TODO TVariadicContainsReference

// TODO TVariadicContainsDuplicate