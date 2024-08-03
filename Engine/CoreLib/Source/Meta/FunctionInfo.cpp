#include "Meta/FunctionInfo.h"

FFunctionParameterInfo::FFunctionParameterInfo(const FStringView name, const FTypeInfo* type)
	: m_Name { name }
	, m_Type { type }
{
}

FFunctionInfo::FFunctionInfo(const FStringView name, const FTypeInfo* returnType, const EFunctionFlags flags)
	: m_Name { name }
	, m_ReturnType { returnType }
	, m_Flags { flags }
{
}

void FFunctionInfo::AddParameter(const FStringView name, const FTypeInfo* type)
{
	(void)m_Parameters.Emplace(name, type);
}

const FFunctionParameterInfo* FFunctionInfo::GetParameter(int32 index) const
{
	if (m_Parameters.IsValidIndex(index))
	{
		return &m_Parameters[index];
	}

	return nullptr;
}

TSpan<const FFunctionParameterInfo> FFunctionInfo::GetParameters() const
{
	return m_Parameters.AsSpan();
}

TErrorOr<FAny> FFunctionInfo::Invoke(TSpan<FAny> parameters)
{
	(void)parameters;
	return MAKE_ERROR("Invoke(params) not yet implemented");
}

TErrorOr<FAny> FFunctionInfo::Invoke(void* instance, TSpan<FAny> parameters)
{
	(void)instance;
	(void)parameters;
	return MAKE_ERROR("Invoke(instance, params) not yet implemented");
}

TErrorOr<FAny> FFunctionInfo::Invoke(const void* instance, TSpan<FAny> parameters)
{
	(void)instance;
	(void)parameters;
	return MAKE_ERROR("Invoke(const instance, params) not yet implemented");
}

void FFunctionInfo::SetInvokeFunction(InvokeFunction invokeFunction)
{
	m_InvokeFunction = MoveTemp(invokeFunction);
}