#include "Meta/TypeInfo.h"

namespace Private
{
	static const FTypeInfo GTypeInfo_void { "void"_sv, 0, 0 };
	const FTypeInfo* TTypeDefinition<void>::Get()
	{
		return &GTypeInfo_void;
	}

	DEFINE_PRIMITIVE_TYPE_DEFINITION(bool)
	DEFINE_PRIMITIVE_TYPE_DEFINITION(int8)
	DEFINE_PRIMITIVE_TYPE_DEFINITION(int16)
	DEFINE_PRIMITIVE_TYPE_DEFINITION(int32)
	DEFINE_PRIMITIVE_TYPE_DEFINITION(int64)
	DEFINE_PRIMITIVE_TYPE_DEFINITION(uint8)
	DEFINE_PRIMITIVE_TYPE_DEFINITION(uint16)
	DEFINE_PRIMITIVE_TYPE_DEFINITION(uint32)
	DEFINE_PRIMITIVE_TYPE_DEFINITION(uint64)
	DEFINE_PRIMITIVE_TYPE_DEFINITION(float)
	DEFINE_PRIMITIVE_TYPE_DEFINITION(double)
	DEFINE_PRIMITIVE_TYPE_DEFINITION(char)
	DEFINE_PRIMITIVE_TYPE_DEFINITION(char8_t)
	DEFINE_PRIMITIVE_TYPE_DEFINITION(char16_t)
	DEFINE_PRIMITIVE_TYPE_DEFINITION(char32_t)
	DEFINE_PRIMITIVE_TYPE_DEFINITION(FStringView)
}

///////////////////////////////////////////////////////////////////////////////
// FTypeInfo

FTypeInfo::FTypeInfo(const FStringView name, const int32 size, const int32 alignment)
	: m_Name { name }
	, m_Size { size }
	, m_Alignment { alignment }
{
}

int32 FTypeInfo::GetAlignment() const
{
	return m_Alignment;
}

FStringView FTypeInfo::GetName() const
{
	return m_Name;
}

int32 FTypeInfo::GetSize() const
{
	return m_Size;
}