#include "Object/Object.h"
#include "Object/ObjectHeader.h"
#include "Object/ObjectPtr.h"
#include "Object/WeakObjectPtr.h"

DEFINE_PRIMITIVE_TYPE_DEFINITION(FWeakObjectPtr)

FWeakObjectPtr::FWeakObjectPtr(UObject* object)
	: m_ObjectHeader { FObjectHeader::FromObject(object) }
{
	if (object == nullptr)
	{
		return;
	}

	m_ObjectHash = object->GetHash();
}

FWeakObjectPtr::FWeakObjectPtr(const FObjectPtr& object)
	: FWeakObjectPtr(object.GetObject())
{
}

UObject* FWeakObjectPtr::GetObject() const
{
	if (m_ObjectHeader != nullptr &&
	    m_ObjectHeader->HasObject() &&
	    m_ObjectHeader->GetObject()->GetHash() == m_ObjectHash)
	{
		return m_ObjectHeader->GetObject();
	}

	return nullptr;
}

bool FWeakObjectPtr::IsA(const FClassInfo* objectClass) const
{
	if (UObject* object = GetObject())
	{
		return object->IsA(objectClass);
	}

	return false;
}

bool FWeakObjectPtr::IsNull() const
{
	return GetObject() == nullptr;
}

bool FWeakObjectPtr::IsValid() const
{
	return GetObject() != nullptr;
}

void FWeakObjectPtr::Reset()
{
	m_ObjectHeader = nullptr;
	m_ObjectHash = INVALID_HASH;
}

UObject* FWeakObjectPtr::operator->() const
{
	return GetObject();
}