//(C) 2025 Alexander Samarin

#pragma once
#include "internal/handle.h"

namespace vsb
{
	template<typename T> class Hnd;

	enum class ObjectHint : uint8_t
	{
		Unspecified,
		Static,
		Scoped,
		Managed
	};


	class Object
	{

		template<typename T> friend class Hnd;

	public:
		//no copying/moving
		Object(const Object&) = delete;
		Object& operator=(const Object&) = delete;
		Object(Object&&) = delete;
		Object& operator=(Object&&) = delete;


	protected:

		explicit Object(ObjectHint objectHint = ObjectHint::Unspecified);
		virtual ~Object();


	private:

		[[nodiscard]] internal::Handle GetHandle() const {return m_handle;}

		internal::Handle RegisterHandle(ObjectHint objectHint);

		internal::Handle m_handle;
	};
}
