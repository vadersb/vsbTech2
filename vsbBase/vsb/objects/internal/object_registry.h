//(C) 2025 Alexander Samarin

#pragma once
#include "vsb/containers/container_defs.h"
#include "vsb/containers/fixed_array.h"
#include "vsb/containers/inplace_stack.h"
#include "vsb/objects/object.h"


namespace vsb
{
	class Object;
}

namespace vsb::internal
{
	constexpr Count ObjectRegistryCapacity = 1024 * 32;

	class ObjectRegistry
	{

		friend class vsb::Object;

	public:

		//queries
		static Count GetCurrentlyRegisteredObjectsCount();


	private:


		Handle RegisterObject(Object* pObject, ObjectHint hint);
		void   UnregisterObject(const Handle& handle);

		[[nodiscard]] Object* GetObject(const Handle& handle) const;
		[[nodiscard]] bool    ValidateHandle(const Handle& handle) const;

		static ObjectRegistry& GetInstance();

		ObjectRegistry();

		struct Entry
		{
			Object* pObject = nullptr;
			uint32_t generation = 0;
			ObjectHint hint = ObjectHint::Unspecified;
		};

		FixedArray<Entry, ObjectRegistryCapacity> m_entries {};
		InplaceStack<uint32_t, ObjectRegistryCapacity> m_freeIndices;

		Count m_exhaustedSlotsCount = 0;
	};
}
