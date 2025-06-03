//(C) 2025 Alexander Samarin


#include "destruction_central.h"
#include "managed_object.h"


namespace vsb
{
	void DestructionCentral::ProcessDefault()
	{
		Process<DefaultDestructionTag>();
	}


	void DestructionCentral::DestructionList::Process()
	{
		for (auto pObject: m_Objects)
		{
			delete pObject;
		}

		m_Objects.Clear();
	}
}
