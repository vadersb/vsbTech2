//(C) 2025 Alexander Samarin

#include "object_registry.h"

namespace vsb::internal
{

	Handle ObjectRegistry::RegisterObject(Object* pObject, ObjectHint hint)
	{
		VSB_ASSERT(pObject != nullptr, "pObject shouldn't be null");
		
		if (m_freeIndices.IsEmpty())
		{
			VSBLOG_CRITICAL("ObjectRegistry is out of free slots!");
			return Handle{}; // Return an empty handle instead of asserting
		}

		const Index index = m_freeIndices.Pop();
		
		// Additional bounds check for safety
		VSB_ASSERT(index >= 0 && index < ObjectRegistryCapacity, "Invalid index from free indices stack");

		auto& entry = m_entries[index];

		entry.generation++;
		entry.pObject = pObject;
		entry.hint = hint;

		return Handle(entry.generation, static_cast<uint32_t>(index));
	}

	void ObjectRegistry::UnregisterObject(const Handle &handle)
	{
		if (handle.IsEmpty())
		{
			VSBLOG_CRITICAL("handle is empty");
			return;
		}

		const auto index = handle.m_index;

		// Bounds checking
		if (index >= ObjectRegistryCapacity)
		{
			VSBLOG_ERROR("Handle index {} is out of bounds", index);
			return;
		}

		auto& entry = m_entries[index];

		if (entry.generation != handle.m_generation)
		{
			VSBLOG_ERROR("Handle generation mismatch! Expected: {}, Got: {}", 
						 entry.generation, handle.m_generation);
			return;
		}

		// Clear the entry
		entry.pObject = nullptr;
		entry.hint = ObjectHint::Unspecified;

		// Only add back to free indices if generation won't overflow on next use
		if (entry.generation < std::numeric_limits<uint32_t>::max())
		{
			m_freeIndices.Push(static_cast<uint32_t>(index));
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

		const auto index = handle.m_index;

		// Bounds checking
		if (index >= ObjectRegistryCapacity)
		{
			VSBLOG_ERROR("Handle index {} is out of bounds", index);
			return nullptr;
		}

		const auto& entry = m_entries[index];

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

		const auto index = handle.m_index;

		// Bounds checking
		if (index >= ObjectRegistryCapacity)
		{
			return false;
		}

		const auto& entry = m_entries[index];
		return entry.generation == handle.m_generation && entry.pObject != nullptr;
	}


	ObjectRegistry & ObjectRegistry::GetInstance()
	{
		static ObjectRegistry instance;
		return instance;
	}


	ObjectRegistry::ObjectRegistry()
	{
		for (Index i = ObjectRegistryCapacity - 1; i >= 0; i--)
		{
			m_freeIndices.Push(static_cast<uint32_t>(i));
		}

		VSB_ASSERT(m_freeIndices.IsFull(), "Free indices stack should be full after initialization");
	}





	Count ObjectRegistry::GetCurrentlyRegisteredObjectsCount()
	{
		return GetInstance().m_freeIndices.GetRemainingCapacity() - GetInstance().m_exhaustedSlotsCount;
	}


	Count ObjectRegistry::GetExhaustedSlotsCount()
	{
		return GetInstance().m_exhaustedSlotsCount;
	}


	Count ObjectRegistry::GetAvailableSlotsCount()
	{
		return GetInstance().m_freeIndices.GetSize();
	}


	ObjectRegistry::ActiveObjectStats ObjectRegistry::GetActiveObjectStats()
	{
		ActiveObjectStats stats{};

		for (const auto& entry : GetInstance().m_entries)
		{
			if (entry.pObject != nullptr)
			{
				switch (entry.hint)
				{
					case ObjectHint::Unspecified:
						stats.unspecifiedObjectsCount++;
						break;

					case ObjectHint::Static:
						stats.staticObjectsCount++;
						break;

					case ObjectHint::Scoped:
						stats.scopedObjectsCount++;
						break;

					case ObjectHint::Managed:
						stats.managedObjectsCount++;
						break;

					default:
						VSBLOG_ERROR("unhandled object hint: {}", static_cast<int>(entry.hint));
						break;
				}
			}
		}

		return stats;
	}


	void ObjectRegistry::WrapUp()
	{
		VSBLOG_INFO("Wrapping up ObjectRegistry.");

		auto stats = GetActiveObjectStats();

		if (stats.scopedObjectsCount > 0)
		{
			VSBLOG_ERROR("Error on ObjectRegistry wrap-up. scopedObjectsCount: {}", stats.scopedObjectsCount);
		}

		if (stats.managedObjectsCount > 0)
		{
			VSBLOG_ERROR("Error on ObjectRegistry wrap-up. managedObjectsCount: {}", stats.managedObjectsCount);
		}
	}


	ObjectRegistryFinalizer::~ObjectRegistryFinalizer()
	{
		ObjectRegistry::WrapUp();
	}
}
