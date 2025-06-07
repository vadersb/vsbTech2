//(C) 2025 Alexander Samarin

#pragma once

#include "vsb/containers/inplace_array.h"


namespace vsb
{
	namespace memory
	{
		enum class AllocationStrategy : uint8_t;
	}

	class ManagedObjectBase;
	struct DefaultDestructionTag;

	template<bool publicDestroy, typename TDestructionTag, memory::AllocationStrategy allocationStrategy>
	class ManagedObject;

	constexpr Count DestructionListCapacity = 1024 * 16;


	class DestructionCentral
	{

		template<bool publicDestroy, typename TDestructionTag, memory::AllocationStrategy allocationStrategy>
		friend class vsb::ManagedObject;

	public:

		static void ProcessDefault();

		static void Uninit()
		{
			ProcessAll();
		}


		template<typename TDestructionTag>
		static void Process()
		{
			auto& destructionList = DestructionList::GetInstance<TDestructionTag>();
			destructionList.Process();
		}

	private:

		static void ProcessAll();


		class DestructionList
		{
		public:

			DestructionList()
			{
				s_DestructionLists.Add(this);
			}


			void Add(ManagedObjectBase* pObject);
			bool Process();


			template<typename TDestructionTag>
			static DestructionList& GetInstance()
			{
				static_assert(std::is_same_v<TDestructionTag, TDestructionTag>, "Template parameter used for specialization");

				static DestructionList instance;
				return instance;
			}

		private:

			bool m_IsProcessing = false;
			bool m_TargetMainList = true;
			InplaceArray<ManagedObjectBase*, DestructionListCapacity> m_ObjectsToDestroy {};
			InplaceArray<ManagedObjectBase*, DestructionListCapacity> m_ObjectsToDestroyExtra {};
		};


		template<typename TDestructionTag>
		static void Schedule(ManagedObjectBase* pObject)
		{
			auto& destructionList = DestructionList::GetInstance<TDestructionTag>();
			destructionList.Add(pObject);
		}

		static constexpr Count DestructionListCount = 256;

		static vsb::InplaceArray<DestructionList*, DestructionListCount> s_DestructionLists;


	};
}
