//(C) 2025 Alexander Samarin

#pragma once


#include "object.h"
#include "destruction_central.h"


namespace vsb
{
	class DestructionCentral;

	class ManagedObject : public Object
	{
		friend class vsb::DestructionCentral;

	public:

		[[nodiscard]] bool IsScheduledForDestruction() const {return m_scheduledForDestruction;}

	protected:

		ManagedObject() : Object(ObjectHint::Managed)
		{}

		~ManagedObject() override = default;


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

		virtual void PassToDestructionCentral()
		{
			ScheduleForDestructionInternal<DefaultDestructionTag>();
		}

		template<typename TDestructionTag>
		void ScheduleForDestructionInternal()
		{
			DestructionCentral::Schedule<TDestructionTag>(this);
		}

	private:

		bool m_scheduledForDestruction = false;
	};

}
