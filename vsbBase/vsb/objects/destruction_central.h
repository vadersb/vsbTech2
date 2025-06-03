//(C) 2025 Alexander Samarin

#pragma once

#include "managed_object.h"

namespace vsb
{
	class ManagedObjectBase;
	struct DefaultDestructionTag;

	template<bool publicDestroy, typename TDestructionTag, memory::AllocationStrategy allocationStrategy>
	class ManagedObject;

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

		static void DestroyManagedObject(ManagedObjectBase* pObject);


		class DestructionList
		{
		public:


			void Add(ManagedObjectBase* pObject)
			{
				m_pObjects.push_back(pObject);

			}


			void Process();


			template<typename TDestructionTag>
			static DestructionList& GetInstance()
			{
				static DestructionList instance;
				return instance;
			}

		private:

			std::vector<ManagedObjectBase*> m_pObjects;
		};


		template<typename TDestructionTag>
		static void Schedule(ManagedObjectBase* pObject)
		{
			auto& destructionList = DestructionList::GetInstance<TDestructionTag>();
			destructionList.Add(pObject);
		}

	};
}