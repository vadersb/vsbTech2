//(C) 2025 Alexander Samarin


#include "destruction_central.h"
#include "managed_object.h"
#include "vsb/log.h"


namespace vsb
{
	std::vector<DestructionCentral::DestructionList*> DestructionCentral::s_DestructionLists {};


	void DestructionCentral::ProcessDefault()
	{
		Process<DefaultDestructionTag>();
	}


	void DestructionCentral::ProcessAll()
	{
		while (true)
		{
			bool hadAnyDeletions = false;

			for (auto* pDestructionList: s_DestructionLists)
			{
				if (pDestructionList->Process())
				{
					hadAnyDeletions = true;
				}
			}

			if (!hadAnyDeletions)
			{
				break;
			}
		}
	}


	void DestructionCentral::DestroyObject(ManagedObjectBase* pObject)
	{
		delete pObject; // NOLINT(*-owning-memory)
	}


	DestructionCentral::DestructionList::DestructionList()
	{
		s_DestructionLists.push_back(this);
		m_ObjectsToDestroy.reserve(1024);
		m_ObjectsToDestroyExtra.reserve(128);
	}


	void DestructionCentral::DestructionList::Add(ManagedObjectBase* pObject)
	{
		if (m_TargetMainList)
		{
			m_ObjectsToDestroy.push_back(pObject);
		}
		else
		{
			m_ObjectsToDestroyExtra.push_back(pObject);
		}
	}


	bool DestructionCentral::DestructionList::Process()
	{
		bool hadAnyDeletions = false;

		if (m_IsProcessing)
		{
			VSBLOG_ERROR("DestructionList::Process called while already processing");
			return false;
		}

		m_IsProcessing = true;

		while (true)
		{
			//main list
			m_TargetMainList = false;

			bool hadDeletions = false;

			if (m_ObjectsToDestroy.empty() == false)
			{
				hadDeletions = true;
				hadAnyDeletions = true;

				for (auto* pObject: m_ObjectsToDestroy)
				{
					DestroyObject(pObject);
				}

				m_ObjectsToDestroy.clear();
			}


			//extra list
			m_TargetMainList = true;

			if (m_ObjectsToDestroyExtra.empty() == false)
			{
				hadDeletions = true;
				hadAnyDeletions = true;

				for (auto* pObject: m_ObjectsToDestroyExtra)
				{
					DestroyObject(pObject);
				}

				m_ObjectsToDestroyExtra.clear();
			}


			if (!hadDeletions)
			{
				break;
			}
		}

		m_IsProcessing = false;

		return hadAnyDeletions;
	}
}
