//(C) 2025 Alexander Samarin


#include "object.h"

#include "vsb/debug.h"

namespace vsb
{
	Object::Object(const ObjectHint objectHint) : m_handle(RegisterHandle(objectHint))
	{
		VSB_ASSERT(m_handle.IsEmpty() == false, "");
	}


	Object::~Object()
	{
		UnregisterHandle(m_handle);
	}


	internal::Handle Object::RegisterHandle(ObjectHint objectHint)
	{
		//todo ObjectRegistry
		return {};
	}


	void Object::UnregisterHandle(internal::Handle handle)
	{
		//todo ObjectRegistry
	}
}
