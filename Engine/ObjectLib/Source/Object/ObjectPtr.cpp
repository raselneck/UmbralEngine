#include "Object/Object.h"
#include "Object/ObjectHeader.h"
#include "Object/ObjectPtr.h"

DEFINE_PRIMITIVE_TYPE_DEFINITION(FObjectPtr)

FObjectPtr::FObjectPtr(UObject* object)
	: m_ObjectHeader { FObjectHeader::FromObject(object) }
{
	if (object == nullptr)
	{
		return;
	}

	m_ObjectHash = object->GetHash();
}

UObject* FObjectPtr::GetObject() const
{
	if (m_ObjectHeader != nullptr &&
	    m_ObjectHeader->HasObject() &&
	    m_ObjectHeader->GetObject()->GetHash() == m_ObjectHash)
	{
		return m_ObjectHeader->GetObject();
	}

	return nullptr;
}

bool FObjectPtr::IsA(const FClassInfo* objectClass) const
{
	if (UObject* object = GetObject())
	{
		return object->IsA(objectClass);
	}

	return false;
}

bool FObjectPtr::IsNull() const
{
	return GetObject() == nullptr;
}

bool FObjectPtr::IsValid() const
{
	return GetObject() != nullptr;
}

void FObjectPtr::Reset()
{
	m_ObjectHeader = nullptr;
	m_ObjectHash = INVALID_HASH;
}

UObject* FObjectPtr::operator->() const
{
	return GetObject();
}