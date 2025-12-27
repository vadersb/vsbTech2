//(C) 2025 Alexander Samarin

#pragma once


#include "object.h"
#include "vsb/memory/allocation_strategy.h"
#include "destruction_central.h"


namespace vsb
{
	class DestructionCentral;

	class ManagedObjectBase : public Object
	{
		friend class vsb::DestructionCentral;

	public:

		[[nodiscard]] bool IsScheduledForDestruction() const {return m_scheduledForDestruction;}

	protected:

		ManagedObjectBase() : Object(ObjectHint::Managed)
		{}

		~ManagedObjectBase() override = default;


		void ScheduleForDestruction()
		{
			if (m_scheduledForDestruction)
			{
				return;
			}

			m_scheduledForDestruction = true;

			OnScheduledForDestruction();
			PassToDestructionCentral();
		}

		virtual void OnScheduledForDestruction() {}
		virtual void PassToDestructionCentral() = 0;

	private:

		bool m_scheduledForDestruction = false;
	};


	struct DefaultDestructionTag;


	template<bool publicDestroy = true, typename TDestructionTag = DefaultDestructionTag>
	class ManagedObject : public ManagedObjectBase
	{
		friend class DestructionCentral;

	public:

		void Destroy() requires publicDestroy
		{
			ScheduleForDestruction();
		}


	protected:

		ManagedObject() = default;
		~ManagedObject() override = default;

		void PassToDestructionCentral() final
		{
			DestructionCentral::Schedule<TDestructionTag>(this);
		}

	};
}
