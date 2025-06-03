//(C) 2025 Alexander Samarin

#pragma once

#include "managed_object.h"

namespace vsb
{
	class ManagedObjectBase;

	class DestructionCentral
	{

		template<>
		friend class ManagedObject;

	public:




	private:

		class DestructionListBase
		{

		};


		class DestructionList : public DestructionListBase
		{
		private:

		};


		template<typename TDestructionTag>
		static void Schedule(ManagedObjectBase* pObject)
		{

		}

	};
}