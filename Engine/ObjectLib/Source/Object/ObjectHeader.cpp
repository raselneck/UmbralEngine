#include "Engine/Logging.h"
#include "Object/Object.h"
#include "Object/ObjectHeader.h"
#include "Object/ObjectHeap.h"

FObjectHeader* FObjectHeader::FromObject(UObject* object)
{
	uint8* headerLocation = reinterpret_cast<uint8*>(object) - sizeof(FObjectHeader);
	return reinterpret_cast<FObjectHeader*>(headerLocation);
}

FObjectHeader* FObjectHeader::GetNextFreeHeader() const
{
	return m_NextFreeHeader;
}

UObject* FObjectHeader::GetObject() const
{
	if (HasObject() == false)
	{
		return nullptr;
	}

	uint8* headerLocation = const_cast<uint8*>(reinterpret_cast<const uint8*>(this));
	return reinterpret_cast<UObject*>(headerLocation + sizeof(FObjectHeader));
}

const FClassInfo* FObjectHeader::GetObjectType() const
{
	if (UObject* object = GetObject())
	{
		return object->GetType();
	}
	return nullptr;
}

bool FObjectHeader::HasObject() const
{
	return m_AllocationTime.GetTicks() != 0;
}

void FObjectHeader::NotifyAllocated(TBadge<FObjectHeapBlock>)
{
	UM_ASSERT(HasObject() == false, "Attempting to re-allocated object header");

	m_NextFreeHeader = nullptr;
	m_AllocationTime = FTimePoint::Now();
}

void FObjectHeader::NotifyDestroyed(TBadge<FObjectHeapBlock>, FObjectHeader* nextHeaderInFreeList)
{
	if (UObject* object = GetObject())
	{
		UM_LOG(Verbose, "Destroying {} allocated at {}", object->GetType()->GetName(), m_AllocationTime.GetTicks());

		constexpr TBadge<FObjectHeader> badge;
		FObjectHeap::DestructObject(badge, object);
	}

	m_NextFreeHeader = nextHeaderInFreeList;
	m_AllocationTime = {};
}

void FObjectHeader::NotifyDestroyed(TBadge<class FObjectHeap>)
{
	UM_LOG(Verbose, "Destroying {} allocated at {}", GetObjectType()->GetName(), GetObjectAllocationTime().GetTicks());

	// If this is being called from the object heap, then we KNOW we have an object
	constexpr TBadge<FObjectHeader> badge;
	FObjectHeap::DestructObject(badge, GetObject());

	m_NextFreeHeader = nullptr;
	m_AllocationTime = {};
}