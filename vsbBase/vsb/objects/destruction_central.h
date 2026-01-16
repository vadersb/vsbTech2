//(C) 2025 Alexander Samarin

#pragma once

#include <vector>


namespace vsb
{
	struct DefaultDestructionTag;
	class ManagedObject;


	class DestructionCentral
	{

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

		static void DestroyObject(ManagedObject* pObject);


		class DestructionList
		{
		public:

			DestructionList();


			void Add(ManagedObject* pObject);
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
			std::vector<ManagedObject*> m_ObjectsToDestroy {};
			std::vector<ManagedObject*> m_ObjectsToDestroyExtra {};
		};


		template<typename TDestructionTag>
		static void Schedule(ManagedObject* pObject)
		{
			auto& destructionList = DestructionList::GetInstance<TDestructionTag>();
			destructionList.Add(pObject);
		}

		static std::vector<DestructionList*> s_DestructionLists;


	};
}
