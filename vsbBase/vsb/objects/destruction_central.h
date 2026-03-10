//(C) 2025 Alexander Samarin

#pragma once

#include <vector>

#include "vsb/debug.h"


namespace vsb
{
	struct DefaultDestructionTag;
	class ManagedObject;
	void VSBInit();

	class DestructionCentral
	{

		friend class vsb::ManagedObject;
		friend void vsb::VSBInit();

	public:

		static void ProcessDefault();

		static void InitDefault()
		{
			DestructionList<DefaultDestructionTag>::Init();
		}

		static void Uninit()
		{
			VSB_CHECK_THREAD();
			ProcessAll();
		}


		template<typename TDestructionTag>
		static void Process()
		{
			auto& destructionList = DestructionList<TDestructionTag>::GetInstance();
			destructionList.Process();
		}

	private:

		static void ProcessAll();

		static void DestroyObject(ManagedObject* pObject);


		class DestructionListBase
		{
		public:
			virtual ~DestructionListBase() = default;

			virtual bool Process() = 0;
		};


		template<typename TDestructionTag>
		class DestructionList : public DestructionListBase
		{
		public:

			DestructionList()
			{
				s_destructionLists.push_back(this);
				m_objectsToDestroy.reserve(1024);
				m_objectsToDestroyExtra.reserve(128);

				s_pInstance = this;
			}


			~DestructionList() final
			{
				s_pInstance = nullptr;
			}


			void Add(ManagedObject* pObject)
			{
				if (m_targetMainList)
				{
					m_objectsToDestroy.push_back(pObject);
				}
				else
				{
					m_objectsToDestroyExtra.push_back(pObject);
				}
			}


			bool Process() override
			{
				bool hadAnyDeletions = false;

				if (m_isProcessing)
				{
					VSBLOG_ERROR("DestructionList::Process called while already processing");
					return false;
				}

				m_isProcessing = true;

				while (true)
				{
					//main list
					m_targetMainList = false;

					bool hadDeletions = false;

					if (m_objectsToDestroy.empty() == false)
					{
						hadDeletions = true;
						hadAnyDeletions = true;

						for (auto* pObject: m_objectsToDestroy)
						{
							DestroyObject(pObject);
						}

						m_objectsToDestroy.clear();
					}


					//extra list
					m_targetMainList = true;

					if (m_objectsToDestroyExtra.empty() == false)
					{
						hadDeletions = true;
						hadAnyDeletions = true;

						for (auto* pObject: m_objectsToDestroyExtra)
						{
							DestroyObject(pObject);
						}

						m_objectsToDestroyExtra.clear();
					}


					if (!hadDeletions)
					{
						break;
					}
				}

				m_isProcessing = false;

				return hadAnyDeletions;
			}


			static void Init()
			{
				static DestructionList instance;
			}


			static DestructionList& GetInstance()
			{
				static_assert(std::is_same_v<TDestructionTag, TDestructionTag>, "Template parameter used for specialization");
				VSB_CHECK_THREAD();

				return *s_pInstance;
			}


		private:

			bool m_isProcessing = false;
			bool m_targetMainList = true;
			std::vector<ManagedObject*> m_objectsToDestroy {};
			std::vector<ManagedObject*> m_objectsToDestroyExtra {};

			static inline DestructionList* s_pInstance {nullptr};
		};


		template<typename TDestructionTag>
		static void Schedule(ManagedObject* pObject)
		{
			auto& destructionList = DestructionList<TDestructionTag>::GetInstance();
			destructionList.Add(pObject);
		}

		static std::vector<DestructionListBase*> s_destructionLists;


	};
}
