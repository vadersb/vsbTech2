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


		template<typename TDestructionTag>
		static void Process()
		{
			auto& destructionList = DestructionList::GetInstance<TDestructionTag>();
			destructionList.Process();
		}

	private:


		class DestructionList
		{
		public:

			void Add(ManagedObjectBase* pObject)
			{
				m_Objects.Add(pObject);
			}


			void Process();


			template<typename TDestructionTag>
			static DestructionList& GetInstance()
			{
				static DestructionList instance;
				return instance;
			}

		private:

			InplaceArray<ManagedObjectBase*, DestructionListCapacity> m_Objects;
		};


		template<typename TDestructionTag>
		static void Schedule(ManagedObjectBase* pObject)
		{
			auto& destructionList = DestructionList::GetInstance<TDestructionTag>();
			destructionList.Add(pObject);
		}

	};
}
