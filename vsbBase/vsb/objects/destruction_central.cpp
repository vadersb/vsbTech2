//(C) 2025 Alexander Samarin


#include "destruction_central.h"


namespace vsb
{
	void DestructionCentral::ProcessDefault()
	{
		Process<DefaultDestructionTag>();
	}


	void DestructionCentral::DestroyManagedObject(ManagedObjectBase* pObject)
	{
		delete pObject;
	}


	void DestructionCentral::DestructionList::Process()
	{
		for (auto pObject: m_Objects)
		{
			DestroyManagedObject(pObject);
		}

		m_Objects.Clear();
	}
}
