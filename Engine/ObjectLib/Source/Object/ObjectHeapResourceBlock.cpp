#include "Engine/Logging.h"
#include "Engine/Object.h"
#include "Engine/ObjectHeap.h"
#include "Engine/ObjectHeapResourceBlock.h"

FObjectHeapResourceBlock::FObjectHeapResourceBlock(const FClassInfo* objectClass)
	: m_ObjectClass { objectClass }
	, m_StrongRefCount { 1 }
	, m_WeakRefCount { 1 }
{
}

FObjectHeapResourceBlock::~FObjectHeapResourceBlock() = default;

void FObjectHeapResourceBlock::AddStrongRef()
{
	++m_StrongRefCount;
}

void FObjectHeapResourceBlock::AddWeakRef()
{
	++m_WeakRefCount;
}

FObjectHeapResourceBlock* FObjectHeapResourceBlock::Allocate(const FClassInfo* objectClass, void* memoryLocation)
{
	FMemory::ConstructObjectAt<FObjectHeapResourceBlock>(memoryLocation, objectClass);
	return reinterpret_cast<FObjectHeapResourceBlock*>(memoryLocation);
}

FObjectHeapResourceBlock* FObjectHeapResourceBlock::FromObject(UObject* object)
{
	if (object == nullptr)
	{
		return nullptr;
	}

	void* resourceBlockLocation = reinterpret_cast<uint8*>(object) - sizeof(FObjectHeapResourceBlock);
	return reinterpret_cast<FObjectHeapResourceBlock*>(resourceBlockLocation);
}

void* FObjectHeapResourceBlock::GetObject()
{
	if (m_HasObjectBeenDestroyed)
	{
		return nullptr;
	}
	return reinterpret_cast<uint8*>(this) + sizeof(FObjectHeapResourceBlock);
}

UObject* FObjectHeapResourceBlock::GetTypedObject()
{
	return reinterpret_cast<UObject*>(GetObject());
}

bool FObjectHeapResourceBlock::HasObject() const
{
	return m_StrongRefCount > 0 && m_HasObjectBeenDestroyed == false;
}

void FObjectHeapResourceBlock::RemoveStrongRef()
{
	if (m_HasObjectBeenDestroyed)
	{
		UM_LOG(Error, "Removing strong reference from already destroyed object");
		return;
	}

	if (m_StrongRefCount <= 0)
	{
		UM_LOG(Error, "Removing too many strong references from object");
		return;
	}

	// Don't decrement the strong ref count just yet so that objects
	// still appear as valid during their Destroyed callbacks
	if (m_StrongRefCount - 1 > 0)
	{
		return;
	}

	UObject* object = GetTypedObject();
	FObjectHeap::DestructObject({}, object);

	--m_StrongRefCount;
	m_HasObjectBeenDestroyed = true;

	RemoveWeakRef();
}

void FObjectHeapResourceBlock::RemoveWeakRef()
{
	if (--m_WeakRefCount > 0)
	{
		return;
	}

	// TODO Free this + object size from the object heap
	FMemory::FreeObject(this);
}

#if WITH_TESTING
int32 FObjectHeapResourceBlock::GetStrongRefCount() const
{
	return m_StrongRefCount;
}

int32 FObjectHeapResourceBlock::GetWeakRefCount() const
{
	return m_WeakRefCount;
}
#endif