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

		constexpr static Count MinStartCapacity = 8;

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


		Array(Array&& moveFrom) noexcept :
		m_count(moveFrom.m_count),
		m_capacity(moveFrom.m_capacity),
		m_data(moveFrom.m_data)
		{
			moveFrom.m_count = 0;
			moveFrom.m_capacity = 0;
			moveFrom.m_data = nullptr;
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
		[[nodiscard]] bool IsFull() const noexcept { return m_count == m_capacity; }
		[[nodiscard]] Count GetSize() const noexcept {return m_count;}
		[[nodiscard]] constexpr Count GetCapacity() const noexcept { return m_capacity; }

		// Iterators for compatibility with STL algorithms
		[[nodiscard]] TValueType* begin() noexcept { return GetPtr(0); }
		[[nodiscard]] const TValueType* begin() const noexcept { return GetPtr(0); }
		[[nodiscard]] const TValueType* cbegin() const noexcept { return GetPtr(0); }

		[[nodiscard]] TValueType* end() noexcept { return GetPtr(m_count); }
		[[nodiscard]] const TValueType* end() const noexcept { return GetPtr(m_count); }
		[[nodiscard]] const TValueType* cend() const noexcept { return GetPtr(m_count); }


		//access
		TValueType& operator[](const Index index) noexcept
		{
			VSB_ASSERT_V(index < m_count && index >= 0, "Index out of bounds", index);
			return m_data[index];
		}


		const TValueType& operator[](const Index index) const noexcept
		{
			VSB_ASSERT_V(index < m_count && index >= 0, "Index out of bounds", index);
			return m_data[index];
		}


		TValueType Get(const Index index, const TValueType defaultValue = TValueType{}) const noexcept
		{
			if (index < m_count && index >= 0)
			{
				return m_data[index];
			}

			return defaultValue;
		}


		void Set(const Index index, const TValueType value) noexcept
		{
			if (index < m_count && index >= 0)
			{
				m_data[index] = value;
			}
		}



		//adding an element to back
		void Add(const TValueType& value) noexcept(std::is_nothrow_copy_constructible_v<TValueType>)
		{
			if (m_count == m_capacity)
			{
				ExpandRegular(m_capacity + 1);
			}

			auto ptr = GetPtr(m_count);
			new (ptr) TValueType(value);
			m_count++;
		}


		void Add(TValueType&& value) noexcept(std::is_nothrow_move_constructible_v<TValueType>)
		{
			if (m_count == m_capacity)
			{
				ExpandRegular(m_capacity + 1);
			}

			auto ptr = GetPtr(m_count);
			new (ptr) TValueType(std::move(value));
			m_count++;
		}


		template<typename... Args>
		TValueType& Emplace(Args&&... args)
		{
			if (m_count == m_capacity)
			{
				ExpandRegular(m_capacity + 1);
			}

			auto ptr = GetPtr(m_count);
			new (ptr) TValueType(std::forward<Args>(args)...);
			return *GetPtr(m_count++);
		}


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


		void ExpandRegular(const Count minNewCapacity)
		{
			const auto newCapacity = CalculateExpandedCapacity(m_capacity, minNewCapacity);
			Expand(newCapacity);
		}


		void Expand(const Count newCapacity)
		{
			VSB_ASSERT(m_capacity < newCapacity, "");
			auto newData = Allocate(newCapacity);
			const auto newCount = m_count;

			for (Index i = 0; i < m_count; ++i)
			{
				std::construct_at(newData + i, std::move(*GetPtr(i)));
				std::destroy_at(GetPtr(i));
			}

			if (m_capacity > 0)
			{
				Deallocate();
			}

			m_count = newCount;
			m_capacity = newCapacity;
			m_data = newData;
		}


		static Count CalculateExpandedCapacity(Count currentCapacity, Count minNewCapacity)
		{
			if (minNewCapacity <= MinStartCapacity)
			{
				return MinStartCapacity;
			}

			Count newCapacity = currentCapacity;

			while (newCapacity < minNewCapacity)
			{
				newCapacity *= 2;
			}

			return newCapacity;
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
