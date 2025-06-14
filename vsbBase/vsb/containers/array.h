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

		template<typename U, memory::AllocationStrategy S>
		friend class Array;


		Array() = default;


		explicit Array(const CapacitySetting capacity) :
		m_capacity(capacity.Capacity),
		m_data(Allocate(m_capacity))
		{
		}


		template<std::convertible_to<TValueType> UValueType, memory::AllocationStrategy otherAllocationStrategy>
		explicit Array(const Array<UValueType, otherAllocationStrategy>& copyFrom) :
		m_count(copyFrom.m_count),
		m_capacity(copyFrom.m_capacity),
		m_data(Allocate(m_capacity))
		{
			for (Index i = 0; i < m_count; ++i)
			{
				std::construct_at(GetPtr(i), *copyFrom.GetPtr(i));
			}
		}


		~Array()
		{
			if (m_data != nullptr)
			{
				Clear();
				Deallocate();
			}
		}


		//queries
		[[nodiscard]] bool IsEmpty() const noexcept {return m_count == 0;}
		[[nodiscard]] Count GetSize() const noexcept {return m_count;}
		[[nodiscard]] constexpr Count GetCapacity() const noexcept { return m_capacity; }

		// Iterators for compatibility with STL algorithms
		[[nodiscard]] TValueType* begin() noexcept { return GetPtr(0); }
		[[nodiscard]] const TValueType* begin() const noexcept { return GetPtr(0); }
		[[nodiscard]] const TValueType* cbegin() const noexcept { return GetPtr(0); }

		[[nodiscard]] TValueType* end() noexcept { return GetPtr(m_count); }
		[[nodiscard]] const TValueType* end() const noexcept { return GetPtr(m_count); }
		[[nodiscard]] const TValueType* cend() const noexcept { return GetPtr(m_count); }


		//modifications
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
			if (size > 0)
			{
				return memory::AllocationSystem::AllocateTyped<TValueType, allocationStrategy>(size);
			}

			return nullptr;
		}


		void Deallocate()
		{
			VSB_ASSERT(m_capacity > 0, "shouldn't deallocate when m_capacity is 0");
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
