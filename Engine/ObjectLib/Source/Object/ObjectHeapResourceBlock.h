#pragma once

#include "Engine/MiscMacros.h"
#include "Memory/SharedResourceBlock.h"
#include "Meta/ClassInfo.h"

/**
 * @brief Defines a resource block to be used with the object heap.
 */
class FObjectHeapResourceBlock final : public Private::ISharedResourceBlock
{
	UM_DISABLE_COPY(FObjectHeapResourceBlock);
	UM_DISABLE_MOVE(FObjectHeapResourceBlock);

	friend class FMemory;

public:

	/**
	 * @brief Destroys this resource block.
	 */
	virtual ~FObjectHeapResourceBlock();

	/** @inheritdoc */
	virtual void AddStrongRef() override;

	/** @inheritdoc */
	virtual void AddWeakRef() override;

	/**
	 * @brief Allocates a resource block for the object heap.
	 *
	 * @param objectSize The size of the object, in bytes.
	 * @param memoryLocation The memory location to construct the resource block at.
	 * @return The resource block.
	 */
	static FObjectHeapResourceBlock* Allocate(const FClassInfo* objectClass, void* memoryLocation);

	/**
	 * @brief Gets the resource block for an object.
	 *
	 * @param object The object.
	 * @return The resource block.
	 */
	static FObjectHeapResourceBlock* FromObject(UObject* object);

	/** @inheritdoc */
	virtual void* GetObject() override;

	/**
	 * @brief Gets the underlying garbage collected object.
	 *
	 * @return The underlying garbage collected object.
	 */
	[[nodiscard]] UObject* GetTypedObject();

	/** @inheritdoc */
	virtual bool HasObject() const override;

	/** @inheritdoc */
	virtual void RemoveStrongRef() override;

	/** @inheritdoc */
	virtual void RemoveWeakRef() override;

#if WITH_TESTING
	/** @copydoc ISharedResourceBlock::GetStrongRefCount() */
	[[nodiscard]] virtual int32 GetStrongRefCount() const override;

	/** @copydoc ISharedResourceBlock::GetWeakRefCount() */
	[[nodiscard]] virtual int32 GetWeakRefCount() const override;
#endif

private:

	/**
	 * @brief Sets default values for this resource block's properties.
	 *
	 * @param objectClass The object's class.
	 */
	FObjectHeapResourceBlock(const FClassInfo* objectClass);

	const FClassInfo* m_ObjectClass = nullptr;
	std::atomic<int32> m_StrongRefCount = 0;
	std::atomic<int32> m_WeakRefCount = 0;
	std::atomic<bool> m_HasObjectBeenDestroyed = false;
};