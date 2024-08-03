#include "Containers/Any.h"
#include <atomic>

FAny::FAny(const FAny& other)
{
	CopyFrom(other);
}

FAny::FAny(FAny&& other) noexcept
{
	MoveFrom(MoveTemp(other));
}

FAny::~FAny()
{
	Reset();
}

const FTypeInfo* FAny::GetValueType() const
{
	if (m_ValueTypeFunction == nullptr)
	{
		return nullptr;
	}

	const FTypeInfo* typeInfo = nullptr;
	m_ValueTypeFunction(EAnyValueOperation::GetType, nullptr, &typeInfo);

	return typeInfo;
}

void FAny::Reset()
{
	if (HasValue() == false)
	{
		return;
	}

	m_ValueTypeFunction(EAnyValueOperation::Destruct, nullptr, m_ValueStorage.GetData());
	FMemory::ZeroOut(m_ValueStorage.GetData(), m_ValueStorage.GetDataSize());

	m_ValueTypeFunction = nullptr;
}

FAny& FAny::operator=(const FAny& other)
{
	if (&other == this)
	{
		return *this;
	}

	Reset();
	CopyFrom(other);

	return *this;
}

FAny& FAny::operator=(FAny&& other) noexcept
{
	if (&other == this)
	{
		return *this;
	}

	MoveFrom(MoveTemp(other));

	return *this;
}

void FAny::CopyFrom(const FAny& other)
{
	m_ValueTypeFunction = other.m_ValueTypeFunction;
	m_ValueStorage.Allocate(other.GetValueTypeSize());

	if (m_ValueTypeFunction == nullptr)
	{
		return;
	}

	m_ValueTypeFunction(EAnyValueOperation::Copy, other.GetValuePointer(), GetValuePointer());
}

void* FAny::GetOrAllocateValuePointer(const int32 valueSize)
{
	Reset();

	m_ValueStorage.Allocate(valueSize);
	return m_ValueStorage.GetData();
}

int32 FAny::GetValueTypeSize() const
{
	if (m_ValueTypeFunction == nullptr)
	{
		return 0;
	}

	const usize result = m_ValueTypeFunction(EAnyValueOperation::GetTypeSize, nullptr, nullptr);
	return static_cast<int32>(result);
}

void FAny::MoveFrom(FAny&& other)
{
	Reset();

	m_ValueTypeFunction = other.m_ValueTypeFunction;
	if (m_ValueTypeFunction == nullptr)
	{
		return;
	}

	const int32 valueTypeSize = GetValueTypeSize();
	m_ValueStorage.Allocate(valueTypeSize);
	m_ValueTypeFunction(EAnyValueOperation::Move, other.m_ValueStorage.GetData(), m_ValueStorage.GetData());

	other.Reset();
}