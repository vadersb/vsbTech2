//(C) 2025 Alexander Samarin

#pragma once
#include "container_defs.h"
#include "vsb/debug.h"


namespace vsb
{
	template<typename TValueType, Count Capacity>
	requires (Capacity > 0)
	class InplaceArray
	{
	public:

		// Default constructor
		InplaceArray() = default;

		//adding an element to back
		void Add(const TValueType& value) noexcept(std::is_nothrow_copy_constructible_v<TValueType>)
		{
			if (m_count == Capacity)
			{
				VSBLOG_CRITICAL("InplaceArray::Add overflow");
				return;
			}

			auto ptr = GetRawPtr(m_count);
			new (ptr) TValueType(value);
			m_count++;
		}


		void Add(TValueType&& value) noexcept(std::is_nothrow_move_constructible_v<TValueType>)
		{
			if (m_count == Capacity)
			{
				VSBLOG_CRITICAL("InplaceArray::Add overflow");
				return;
			}

			auto ptr = GetRawPtr(m_count);
			new (ptr) TValueType(std::move(value));
			m_count++;
		}


		bool TryAdd(const TValueType& copyFrom) noexcept(std::is_nothrow_copy_constructible_v<TValueType>)
		{
			if (m_count == Capacity)
			{
				return false;
			}

			auto ptr = GetRawPtr(m_count);
			new (ptr) TValueType(copyFrom);
			m_count++;
			return true;
		}


		bool TryAdd(TValueType&& moveFrom)  noexcept(std::is_nothrow_move_constructible_v<TValueType>)
		{
			if (m_count == Capacity)
			{
				return false;
			}

			auto ptr = GetRawPtr(m_count);
			new (ptr) TValueType(std::move(moveFrom));
			m_count++;
			return true;
		}


		template<typename... Args>
		TValueType& Emplace(Args&&... args)
		{
			if (m_count == Capacity)
			{
				VSBLOG_CRITICAL("InplaceArray::Add overflow");
				return {};
			}

			auto ptr = GetRawPtr(m_count);
			new (ptr) TValueType(std::forward<Args>(args)...);
			return *GetPtr(m_count++);
		}


		template<typename... Args>
		bool TryEmplace(Args&&... args)
		{
			if (m_count == Capacity)
			{
				return false;
			}

			auto ptr = GetRawPtr(m_count);
			new (ptr) TValueType(std::forward<Args>(args)...);
			m_count++;
			return true;
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



		~InplaceArray()
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


		//modifications
		void Fill(const TValueType value = TValueType{}) noexcept
		{
			for (Index i = 0; i < m_count; ++i)
			{
				m_data[i] = value;
			}
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