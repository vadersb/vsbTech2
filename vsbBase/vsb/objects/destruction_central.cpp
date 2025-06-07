//(C) 2025 Alexander Samarin


#include "destruction_central.h"
#include "managed_object.h"


namespace vsb
{
	vsb::InplaceArray<DestructionCentral::DestructionList*, 256> DestructionCentral::s_DestructionLists {};


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


	void DestructionCentral::DestructionList::Add(ManagedObjectBase* pObject)
	{
		if (m_TargetMainList)
		{
			m_ObjectsToDestroy.Add(pObject);
		}
		else
		{
			m_ObjectsToDestroyExtra.Add(pObject);
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

			if (m_ObjectsToDestroy.IsEmpty() == false)
			{
				hadDeletions = true;
				hadAnyDeletions = true;

				for (auto* pObject: m_ObjectsToDestroy)
				{
					delete pObject;
				}

				m_ObjectsToDestroy.Clear();
			}


			//extra list
			m_TargetMainList = true;

			if (m_ObjectsToDestroyExtra.IsEmpty() == false)
			{
				hadDeletions = true;
				hadAnyDeletions = true;

				for (auto* pObject: m_ObjectsToDestroyExtra)
				{
					delete pObject;
				}

				m_ObjectsToDestroyExtra.Clear();
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
