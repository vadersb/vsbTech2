//(C) 2025 Alexander Samarin

#include "object_registry.h"

namespace vsb::internal
{



	Handle ObjectRegistry::RegisterObject(Object* pObject, ObjectHint hint)
	{
		VSB_ASSERT(pObject != nullptr, "pObject shoudn't be null");
		VSB_ASSERT(m_freeIndices.IsEmpty() == false, "ObjectRegistry is out of free slots!");

		const Index index = m_freeIndices.Pop();

		auto& entry = m_entries[index];

		entry.generation++;
		entry.pObject = pObject;
		entry.hint = hint;

		return Handle(entry.generation, index);
	}


	void ObjectRegistry::UnregisterObject(const Handle &handle)
	{
		if (handle.IsEmpty())
		{
			VSBLOG_CRITICAL("handle is empty");
			return;
		}

		auto index = handle.m_index;

		auto& entry = m_entries[index];

		if (entry.generation != handle.m_generation)
		{
			VSBLOG_ERROR("handle generation mismatch!");
			return;
		}

		entry.pObject = nullptr;
		entry.hint = ObjectHint::Unspecified;

		if (entry.generation < std::numeric_limits<uint32_t>::max())
		{
			m_freeIndices.Push(index);
		}
		else
		{
			m_exhaustedSlotsCount++;
			VSBLOG_DEBUG("ObjectRegistry slot exhausted. index: {}, total exhausted slots: {}", index, m_exhaustedSlotsCount);
		}
	}


	Object* ObjectRegistry::GetObject(const Handle &handle) const
	{
		if (handle.IsEmpty())
		{
			return nullptr;
		}

		auto index = handle.m_index;
		auto& entry = m_entries[index];

		if (entry.generation != handle.m_generation)
		{
			return nullptr;
		}

		return entry.pObject;
	}


	bool ObjectRegistry::ValidateHandle(const Handle &handle) const
	{
		if (handle.IsEmpty())
		{
			return false;
		}

		auto index = handle.m_index;
		auto& entry = m_entries[index];

		return entry.generation == handle.m_generation;
	}


	ObjectRegistry & ObjectRegistry::GetInstance()
	{
		static ObjectRegistry instance;
		return instance;
	}


	ObjectRegistry::ObjectRegistry()
	{
		for (int32_t i = ObjectRegistryCapacity - 1; i >= 0; i--)
		{
			m_freeIndices.Push(i);
		}

		VSB_ASSERT(m_freeIndices.IsFull(), "Free indices stack should be full after initialization");
	}


	Count ObjectRegistry::GetCurrentlyRegisteredObjectsCount()
	{
		return GetInstance().m_freeIndices.GetRemainingCapacity();
	}
}
