//(C) 2025 Alexander Samarin

#pragma once
#include "container_defs.h"
#include "vsb/debug.h"
#include "vsb/memory/allocation_strategy.h"
#include "vsb/memory/memory.h"


namespace vsb
{
	template<typename TValueType, memory::AllocationStrategy allocationStrategy = memory::AllocationStrategy::Std>
	class Array
	{
	public:

		Array() = default;


		explicit Array(const CapacitySetting capacity) :
		m_capacity(capacity.Capacity),
		m_data(Allocate(m_capacity))
		{
		}


		~Array()
		{
			if (m_data != nullptr)
			{
				Clear();
				Deallocate();
			}
		}


		void Clear() noexcept
		{
			if constexpr (!std::is_trivially_destructible_v<TValueType>)
			{
				for (Index i = 0; i < m_count; ++i)
				{
					std::destroy_at(GetPtr(i));
				}
			}

			m_count = 0;
		}


		//modifications
		void Fill(const TValueType value = TValueType{}) noexcept
		{
			for (Index i = 0; i < m_count; ++i)
			{
				m_data[i] = value;
			}
		}


	private:




		static TValueType* Allocate(const Count size)
		{
			return memory::AllocationSystem::AllocateTyped<TValueType, allocationStrategy>(size);
		}


		void Deallocate()
		{
			memory::AllocationSystem::DeallocateTyped<TValueType, allocationStrategy>(m_data, m_capacity);
			m_data = nullptr;
			m_count = 0;
			m_capacity = 0;
		}


		TValueType* GetPtr(const Index index) noexcept
		{
			return m_data + index;
		}


		const TValueType* GetPtr(const Index index) const noexcept
		{
			return m_data + index;
		}

		Count m_count {0};
		Count m_capacity {0};
		TValueType* m_data {nullptr};
	};
}
