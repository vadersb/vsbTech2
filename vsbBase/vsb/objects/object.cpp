//(C) 2025 Alexander Samarin


#include "object.h"

#include "internal/object_registry.h"
#include "vsb/debug.h"

namespace vsb
{
	Object::Object(const ObjectHint objectHint) : m_handle(RegisterHandle(objectHint))
	{
		VSB_ASSERT(m_handle.IsEmpty() == false, "");
	}


	Object::~Object()
	{
		if (internal::ObjectRegistry::s_pInstance == nullptr)
		{
			return;
		}

		internal::ObjectRegistry::GetInstance().UnregisterObject(m_handle);
	}


	internal::Handle Object::RegisterHandle(ObjectHint objectHint)
	{
		if (internal::ObjectRegistry::s_pInstance == nullptr)
		{
			VSB_ASSERT(false, "ObjectRegistry is terminated");
			return internal::Handle::Empty;
		}

		return internal::ObjectRegistry::GetInstance().RegisterObject(this, objectHint);
	}
}
