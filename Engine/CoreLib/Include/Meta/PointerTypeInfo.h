#pragma once

#include "Containers/String.h"
#include "Meta/TypeInfo.h"

/**
 * @brief Defines type information for an array.
 */
class FPointerTypeInfo : public FTypeInfo
{
public:

	/**
	 * @brief Sets default values for this pointer type info's properties.
	 *
	 * @param valueType The type of the underlying value.
	 */
	explicit FPointerTypeInfo(const FTypeInfo* valueType);

	/**
	 * @brief Destroys this pointer type info.
	 */
	virtual ~FPointerTypeInfo() override = default;

	/**
	 * @brief Gets the underlying value type.
	 *
	 * @return The underlying value type.
	 */
	[[nodiscard]] const FTypeInfo* GetValueType() const;

private:

	FString m_FormattedName;
	const FTypeInfo* m_ValueType = nullptr;
};

namespace Private
{
	template<typename T>
	struct TTypeDefinition<T*>
	{
		static const FPointerTypeInfo* Get()
		{
			static const FPointerTypeInfo arrayTypeInfo { ::GetType<T>() };
			return &arrayTypeInfo;
		}
	};
}