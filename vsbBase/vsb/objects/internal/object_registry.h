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

	class ObjectRegistryFinalizer
	{
	public:
		~ObjectRegistryFinalizer();
	};


	class ObjectRegistry
	{
		friend class ObjectRegistryFinalizer;
		friend class vsb::Object;

	public:

		struct ActiveObjectStats
		{
			Count unspecifiedObjectsCount = 0;
			Count staticObjectsCount = 0;
			Count scopedObjectsCount = 0;
			Count managedObjectsCount = 0;
		};

		//queries
		static Count GetCurrentlyRegisteredObjectsCount();
		static Count GetExhaustedSlotsCount();
		static Count GetAvailableSlotsCount();
		static ActiveObjectStats GetActiveObjectStats();

	private:

		Handle RegisterObject(Object* pObject, ObjectHint hint);
		void   UnregisterObject(const Handle& handle);

		[[nodiscard]] Object* GetObject(const Handle& handle) const;
		[[nodiscard]] bool    ValidateHandle(const Handle& handle) const;

		static ObjectRegistry& GetInstance();

		ObjectRegistry();

		static void WrapUp();

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
