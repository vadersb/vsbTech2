//(C) 2025 Alexander Samarin

#pragma once


#include <algorithm>
#include <cstddef>
#include <memory> // for std::launder and std::destroy_at

#include "container_defs.h"
#include "vsb/debug.h"


namespace vsb
{
	template <typename TValueType, Count Capacity>
    requires (Capacity > 0)
	class InplaceStack
	{
	public:
		// Default constructor
		InplaceStack() = default;

		// Delete copy and move operations
		InplaceStack(const InplaceStack&) = delete;
		InplaceStack& operator=(const InplaceStack&) = delete;
		InplaceStack(InplaceStack&&) = delete;
		InplaceStack& operator=(InplaceStack&&) = delete;


		void Push(const TValueType& value) noexcept(std::is_nothrow_copy_constructible_v<TValueType>)
		{
			VSB_ASSERT(m_count < Capacity, "InPlaceStack overflow");

			auto ptr = GetRawPtr(m_count);
			new (ptr) TValueType(value);
			m_count++;
		}


		void Push(TValueType&& value) noexcept(std::is_nothrow_move_constructible_v<TValueType>)
		{
			VSB_ASSERT(m_count < Capacity, "InPlaceStack overflow");
			auto ptr = GetRawPtr(m_count);
			new (ptr) TValueType(std::move(value));
			m_count++;
		}


		template<typename... Args>
		TValueType& Emplace(Args&&... args)
		{
			VSB_ASSERT(m_count < Capacity, "InPlaceStack overflow");
			auto ptr = GetRawPtr(m_count);
			new (ptr) TValueType(std::forward<Args>(args)...);
			return *GetPtr(m_count++);
		}


		TValueType Pop() noexcept(std::is_nothrow_move_constructible_v<TValueType>)
		{
			VSB_ASSERT(m_count > 0, "InPlaceStack underflow");
			--m_count;
			
			// Create a copy/move before destroying
			TValueType* ptr = GetPtr(m_count);
			TValueType value = std::move(*ptr);
			
			if constexpr (!std::is_trivially_destructible_v<TValueType>)
			{
				std::destroy_at(ptr);
			}
			return value;
		}


		void PopLite() noexcept
		{
			VSB_ASSERT(m_count > 0, "InPlaceStack underflow");
			if constexpr (!std::is_trivially_destructible_v<TValueType>)
			{
				std::destroy_at(GetPtr(m_count - 1));
			}
			m_count--;
		}


		TValueType& Peek() noexcept
		{
			VSB_ASSERT(m_count > 0, "InPlaceStack underflow");
			return *GetPtr(m_count - 1);
		}


		const TValueType& Peek() const noexcept
		{
			VSB_ASSERT(m_count > 0, "InPlaceStack underflow");
			return *GetPtr(m_count - 1);
		}


		//queries
		[[nodiscard]] bool IsEmpty() const noexcept {return m_count == 0;}
		[[nodiscard]] bool IsFull() const noexcept { return m_count == Capacity; }
		[[nodiscard]] Count GetSize() const noexcept {return m_count;}
		[[nodiscard]] static constexpr Count GetCapacity() noexcept { return Capacity; }
		[[nodiscard]] Count GetRemainingCapacity() const noexcept { return Capacity - m_count; }

		// Iterators for compatibility with STL algorithms
		[[nodiscard]] TValueType* begin() noexcept { return GetPtr(0); }
		[[nodiscard]] const TValueType* begin() const noexcept { return GetPtr(0); }
		[[nodiscard]] const TValueType* cbegin() const noexcept { return GetPtr(0); }

		[[nodiscard]] TValueType* end() noexcept { return GetPtr(m_count); }
		[[nodiscard]] const TValueType* end() const noexcept { return GetPtr(m_count); }
		[[nodiscard]] const TValueType* cend() const noexcept { return GetPtr(m_count); }


		~InplaceStack()
		{
			Clear();
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


	private:


		void* GetRawPtr(const Index index) noexcept
		{
			return static_cast<void*>(m_data + index * sizeof(TValueType));
		}


		TValueType* GetPtr(const Index index) noexcept
		{
			return std::launder(reinterpret_cast<TValueType*>(m_data + index * sizeof(TValueType)));
		}


		const TValueType* GetPtr(const Index index) const noexcept
		{
			return std::launder(reinterpret_cast<const TValueType*>(m_data + index * sizeof(TValueType)));
		}



		static constexpr Count DataCapacity = Capacity * sizeof(TValueType);

		alignas(TValueType) std::byte m_data[DataCapacity] = {};

		Count m_count = 0;
	};
}