//(C) 2025 Alexander Samarin

#include "object_registry.h"
#include "vsb/debug.h"
#include "vsb/containers/container_utils.h"

namespace vsb::internal
{
	ObjectRegistry* ObjectRegistry::s_pInstance = nullptr;

	Handle ObjectRegistry::RegisterObject(Object* pObject, ObjectHint hint)
	{
		VSB_ASSERT(pObject != nullptr, "pObject shouldn't be null");
		
		if (m_freeIndices.empty())
		{
			ExpandStorage();
		}

		const uint32_t index = m_freeIndices.top();
		m_freeIndices.pop();
		
		// Additional bounds check for safety
		VSB_ASSERT(index < m_entries.size(), "Invalid index from free indices stack");

		auto& entry = m_entries.at(index);

		entry.generation++;
		entry.pObject = pObject;
		entry.hint = hint;

		m_currentlyRegisteredObjectsCount++;

		return Handle(index, entry.generation);
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
		if (index >= m_entries.size())
		{
			VSBLOG_ERROR("Handle index {} is out of bounds", index);
			return;
		}

		auto& entry = m_entries.at(index);

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
			m_freeIndices.push(static_cast<uint32_t>(index));
		}
		else
		{
			m_exhaustedSlotsCount++;
			VSBLOG_DEBUG("ObjectRegistry slot exhausted. index: {}, total exhausted slots: {}", index, m_exhaustedSlotsCount);
		}

		m_currentlyRegisteredObjectsCount--;
	}


	Object* ObjectRegistry::GetObject(const Handle &handle) const
	{
		if (handle.IsEmpty())
		{
			return nullptr;
		}

		const auto index = handle.m_index;

		// Bounds checking
		if (index >= m_entries.size())
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
		if (index >= m_entries.size())
		{
			return false;
		}

		const auto& entry = m_entries[index];
		return entry.generation == handle.m_generation && entry.pObject != nullptr;
	}


	ObjectRegistry & ObjectRegistry::GetInstance()
	{
		VSB_ASSERT(s_pInstance != nullptr, "ObjectRegistry instance is null!");
		VSB_CHECK_THREAD();
		return *s_pInstance;
	}


	void ObjectRegistry::ExpandStorage()
	{
		for (size_t i = 0; i < ObjectRegistryCapacity; i++)
		{
			m_entries.emplace_back();
			m_freeIndices.push(static_cast<uint32_t>(m_entries.size() - 1));
		}
	}


	ObjectRegistry::ObjectRegistry()// :
	//m_freeIndices(std::move(vsb::ReserveEmptyVector<uint32_t>(ObjectRegistryCapacity)))
	{
		m_entries.reserve(ObjectRegistryCapacity);

		// Reserve capacity for the underlying vector of m_freeIndices
		// This requires a small workaround since std::stack doesn't expose reserve().
		std::vector<uint32_t> initialIndices;
		initialIndices.reserve(ObjectRegistryCapacity);
		for (uint32_t i = 0; i < ObjectRegistryCapacity; i--)
		{
			initialIndices.push_back(ObjectRegistryCapacity - 1 - i);
		}

		m_freeIndices = std::stack(std::move(initialIndices));

		s_pInstance = this;
	}


	size_t ObjectRegistry::GetCurrentlyRegisteredObjectsCount()
	{
		return GetInstance().m_currentlyRegisteredObjectsCount;
	}


	size_t ObjectRegistry::GetExhaustedSlotsCount()
	{
		return GetInstance().m_exhaustedSlotsCount;
	}


	size_t ObjectRegistry::GetAvailableSlotsCount()
	{
		return GetInstance().m_freeIndices.size();
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

					case ObjectHint::Singleton:
						stats.singletonObjectsCount++;
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


	void ObjectRegistry::Init()
	{
		static ObjectRegistry staticInstance;
	}


	void ObjectRegistry::WrapUp()
	{
		VSBLOG_INFO("Wrapping up ObjectRegistry.");

		if (s_pInstance == nullptr)
		{
			VSBLOG_CRITICAL("ObjectRegistry is already terminated!");
			return;
		}

		auto stats = GetActiveObjectStats();

		if (stats.unspecifiedObjectsCount > 0)
		{
			VSBLOG_ERROR("Error on ObjectRegistry wrap-up. unspecifiedObjectsCount: {}", stats.unspecifiedObjectsCount);
		}

		if (stats.scopedObjectsCount > 0)
		{
			VSBLOG_ERROR("Error on ObjectRegistry wrap-up. scopedObjectsCount: {}", stats.scopedObjectsCount);
		}

		if (stats.managedObjectsCount > 0)
		{
			VSBLOG_ERROR("Error on ObjectRegistry wrap-up. managedObjectsCount: {}", stats.managedObjectsCount);
		}

		if (stats.singletonObjectsCount > 0)
		{
			VSBLOG_ERROR("Error on ObjectRegistry wrap-up. singletonObjectsCount: {}", stats.singletonObjectsCount);
		}

		//static objects are fine
	}


	ObjectRegistryFinalizer::~ObjectRegistryFinalizer()
	{
		ObjectRegistry::WrapUp();
	}


	ObjectRegistry::~ObjectRegistry()
	{
		s_pInstance = nullptr;
	}
}
