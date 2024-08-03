#pragma once

#include "Containers/Array.h"
#include "Containers/StaticArray.h"
#include "Containers/String.h"
#include "Meta/TypeInfo.h"

/**
 * @brief Defines type information for an array.
 */
class FArrayTypeInfo : public FTypeInfo
{
public:

	/**
	 * @brief Sets default values for this array type info's properties.
	 *
	 * @param size The type's size.
	 * @param alignment The type's alignment.
	 * @param elementType The type of each element in the array.
	 */
	explicit FArrayTypeInfo(int32 size, int32 alignment, const FTypeInfo* elementType);

	/**
	 * @brief Destroys this array type info.
	 */
	virtual ~FArrayTypeInfo() override = default;

	/**
	 * @brief Gets the type of each element in the array.
	 *
	 * @return The type of each element in the array.
	 */
	[[nodiscard]] const FTypeInfo* GetElementType() const;

private:

	/**
	 * @brief Generates the name for the array type info.
	 *
	 * @param targetString The string to assign to.
	 * @param elementType The element type owned by the array.
	 * @return \p targetString as a string view.
	 */
	static FStringView GenerateArrayTypeName(FString& targetString, const FTypeInfo* elementType);

	FString m_FormattedName;
	const FTypeInfo* m_ElementType = nullptr;
};

/**
 * @brief Defines type information for a static array.
 */
class FStaticArrayTypeInfo : public FTypeInfo
{
public:

	/**
	 * @brief Sets default values for this array type info's properties.
	 *
	 * @param size The type's size.
	 * @param alignment The type's alignment.
	 * @param elementType The type of each element in the array.
	 */
	explicit FStaticArrayTypeInfo(const FStringView name, const int32 size, const int32 alignment, const FTypeInfo* elementType)
		: FTypeInfo(name, size, alignment)
		, m_ElementType { elementType }
	{
	}

	/**
	 * @brief Destroys this static array type info.
	 */
	virtual ~FStaticArrayTypeInfo() override = default;

	/**
	 * @brief Gets the type of each element in the array.
	 *
	 * @return The type of each element in the array.
	 */
	[[nodiscard]] const FTypeInfo* GetElementType() const
	{
		return m_ElementType;
	}

	/**
	 * @brief Gets the number of elements in this static array.
	 *
	 * @return The number of elements in this static array.
	 */
	[[nodiscard]] virtual int32 GetNumElements() const = 0;

private:

	const FTypeInfo* m_ElementType = nullptr;
};

/**
 * @brief Defines type information for a static array.
 *
 * @tparam N The number of elements in the static array.
 */
template<int32 N>
class TStaticArrayTypeInfo : public FStaticArrayTypeInfo
{
public:

	/**
	 * @brief Sets default values for this array type info's properties.
	 *
	 * @param size The type's size.
	 * @param alignment The type's alignment.
	 * @param elementType The type of each element in the array.
	 */
	explicit TStaticArrayTypeInfo(const int32 size, const int32 alignment, const FTypeInfo* elementType)
		: FStaticArrayTypeInfo(GenerateStaticArrayTypeName(m_FormattedName, elementType), size, alignment, elementType)
	{
	}

	/**
	 * @brief Destroys this static array type info.
	 */
	virtual ~TStaticArrayTypeInfo() override = default;

	/** @copydoc FStaticArrayTypeInfo::GetNumElements */
	[[nodiscard]] virtual int32 GetNumElements() const override
	{
		return N;
	}

private:

	/**
	 * @brief Generates the name for the static array type info.
	 *
	 * @param targetString The string to assign to.
	 * @param elementType The element type owned by the array.
	 * @return \p targetString as a string view.
	 */
	static FStringView GenerateStaticArrayTypeName(FString& targetString, const FTypeInfo* elementType)
	{
		targetString = FString::Format("TStaticArray<{}, {}>"_sv, elementType->GetName(), N);
		return targetString.AsStringView();
	}

	FString m_FormattedName;
};

namespace Private
{
	template<typename T>
	struct TTypeDefinition<TArray<T>>
	{
		using ArrayType = TArray<T>;

		static const FArrayTypeInfo* Get()
		{
			static const FArrayTypeInfo arrayTypeInfo { sizeof(ArrayType), alignof(ArrayType), ::GetType<T>() };
			return &arrayTypeInfo;
		}
	};

	template<typename T, int32 N>
	struct TTypeDefinition<TStaticArray<T, N>>
	{
		using ArrayType = TStaticArray<T, N>;

		static const FStaticArrayTypeInfo* Get()
		{
			static const TStaticArrayTypeInfo<N> arrayTypeInfo { sizeof(ArrayType), alignof(ArrayType), ::GetType<T>() };
			return &arrayTypeInfo;
		}
	};
}