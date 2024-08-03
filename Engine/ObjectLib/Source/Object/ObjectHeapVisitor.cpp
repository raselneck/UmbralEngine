#include "Object/ObjectHeapVisitor.h"
#include "Object/ObjectPtr.h"
#include "Object/WeakObjectPtr.h"

void FObjectHeapVisitor::Visit(const FObjectPtr& objectPtr)
{
	if (UObject* object = objectPtr.GetObject())
	{
		Visit(object);
	}
}

void FObjectHeapVisitor::Visit(const FWeakObjectPtr& objectPtr)
{
	// Soft object pointers are basically weak pointers, so we don't HAVE to visit them to keep them alive
	(void)objectPtr;
}