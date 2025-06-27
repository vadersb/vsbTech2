//(C) 2025 Alexander Samarin

#pragma once

#include "object.h"
#include "vsb/debug.h"

namespace vsb
{
	template<typename TObject>
	class SingleInstanceObject : public Object
	{
	public:

		static TObject& GetCurrentInstance()
		{
			VSB_ASSERT(s_CurrentInstance != nullptr, "current instance is null!");
			return *s_CurrentInstance;
		}


		static TObject* GetCurrentInstancePtr()
		{
			return s_CurrentInstance;
		}


	protected:


		explicit SingleInstanceObject(ObjectHint objectHint = ObjectHint::Unspecified) : Object(objectHint)
		{
			VSB_ASSERT(objectHint != ObjectHint::Singleton, "objectHint shouldn't be Singleton");
			VSB_ASSERT(objectHint != ObjectHint::Static, "objectHint shouldn't be Static");

			VSB_ASSERT(s_CurrentInstance == nullptr, "Single instance already exists");
			s_CurrentInstance = static_cast<TObject*>(this);
		}


		~SingleInstanceObject() override
		{
			VSB_ASSERT(s_CurrentInstance == static_cast<TObject*>(this), "Single instance not exists");
			s_CurrentInstance = nullptr;
		}


	private:

		static inline TObject* s_CurrentInstance {nullptr};
	};
}