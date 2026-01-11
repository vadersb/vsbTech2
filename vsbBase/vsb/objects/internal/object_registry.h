//(C) 2025 Alexander Samarin

#pragma once
#include <cstddef>
#include <stack>
#include <vector>
#include "vsb/objects/object.h"


namespace vsb
{
	class Object;
	template<typename T> class Hnd;
	template<typename T> class Ptr;
	template<typename T> class SafePtr;
}

namespace vsb::internal
{
	constexpr std::size_t ObjectRegistryCapacity = 1024 * 8;

	class ObjectRegistryFinalizer
	{
	public:
		~ObjectRegistryFinalizer();
	};


	class ObjectRegistry
	{
		friend class ObjectRegistryFinalizer;
		friend class vsb::Object;
		template<typename T> friend class vsb::Hnd;
		template<typename T> friend class vsb::Ptr;
		template<typename T> friend class vsb::SafePtr;

	public:

		~ObjectRegistry();

		struct ActiveObjectStats
		{
			std::size_t unspecifiedObjectsCount = 0;
			std::size_t staticObjectsCount = 0;
			std::size_t singletonObjectsCount = 0;
			std::size_t scopedObjectsCount = 0;
			std::size_t managedObjectsCount = 0;
		};

		//queries
		static size_t GetCurrentlyRegisteredObjectsCount();
		static size_t GetExhaustedSlotsCount();
		static size_t GetAvailableSlotsCount();
		static ActiveObjectStats GetActiveObjectStats();

	private:

		Handle RegisterObject(Object* pObject, ObjectHint hint);
		void   UnregisterObject(const Handle& handle);

		[[nodiscard]] Object* GetObject(const Handle& handle) const;
		[[nodiscard]] bool    ValidateHandle(const Handle& handle) const;

		static bool s_IsTerminated;
		static ObjectRegistry& GetInstance();

		void ExpandStorage();

		ObjectRegistry();


		static void WrapUp();

		struct Entry
		{
			Object* pObject = nullptr;
			uint32_t generation = 0;
			ObjectHint hint = ObjectHint::Unspecified;
		};

		//FixedArray<Entry, ObjectRegistryCapacity> m_entries {};
		std::vector<Entry> m_entries {ObjectRegistryCapacity, Entry()};
		std::stack<uint32_t, std::vector<uint32_t>> m_freeIndices {};

		size_t m_exhaustedSlotsCount = 0;
		size_t m_currentlyRegisteredObjectsCount = 0;
	};
}
