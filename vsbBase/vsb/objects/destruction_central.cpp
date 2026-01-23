//(C) 2025 Alexander Samarin


#include "destruction_central.h"
#include "managed_object.h"
#include "vsb/log.h"


namespace vsb
{
	std::vector<DestructionCentral::DestructionListBase*> DestructionCentral::s_DestructionLists {};


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


	void DestructionCentral::DestroyObject(ManagedObject* pObject)
	{
		delete pObject; // NOLINT(*-owning-memory)
	}
}
