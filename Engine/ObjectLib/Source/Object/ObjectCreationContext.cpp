#include "Object/ObjectCreationContext.h"

bool FObjectCreationContext::HasParameter(const FStringView name) const
{
	return m_Parameters.ContainsKey(name);
}

FAny* FObjectCreationContext::GetParameterAsAny(const FStringView name) const
{
	// TODO Nasty, bad. Can probably modify Find to avoid the use of const_cast
	const FAny* value = m_Parameters.Find(name);
	return const_cast<FAny*>(value);
}

bool FObjectCreationContext::SetParameterAsAny(const FStringView name, FAny value)
{
	return m_Parameters.Add(name, MoveTemp(value));
}