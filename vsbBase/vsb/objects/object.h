//(C) 2025 Alexander Samarin

#pragma once
#include "internal/handle.h"

namespace vsb
{
	enum class ObjectHint
	{
		Unspecified,
		Static,
		Scoped,
		Managed
	};


	class Object
	{
	protected:

		explicit Object(ObjectHint objectHint = ObjectHint::Unspecified);

		//no copying/moving
		Object(const Object&) = delete;
		Object& operator=(const Object&) = delete;
		Object(Object&&) = delete;
		Object& operator=(Object&&) = delete;

		virtual ~Object();


	private:

		static internal::Handle RegisterHandle(ObjectHint objectHint);
		static void UnregisterHandle(internal::Handle handle);

		internal::Handle m_handle;
	};
}
