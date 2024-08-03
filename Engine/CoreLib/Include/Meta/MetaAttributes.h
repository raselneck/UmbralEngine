#pragma once

namespace Private
{
	class FAttribute
	{
	};

	class FAbstractAttribute : public FAttribute
	{
	};

	class FChildOfAttribute : public FAttribute
	{
	};
}

#define UM_META_ATTRIBUTE_TYPE(Name) F##Name##Attribute