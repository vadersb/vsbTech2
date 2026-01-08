//(C) 2025 Alexander Samarin

#pragma once
#include "container_defs.h"
#include "vsb/debug.h"
#include <memory>
#include <type_traits>


namespace vsb
{
	template<typename TValueType, Count Capacity>
	requires (Capacity > 0)
	class InplaceArray
	{
	public:

		// Default constructor
		InplaceArray() = default;

		// Copy constructor
		InplaceArray(const InplaceArray& other) noexcept(std::is_nothrow_copy_constructible_v<TValueType>)
		{
            if constexpr (std::is_trivially_copyable_v<TValueType>)
            {
                std::memcpy(m_data, other.m_data, other.m_count * sizeof(StorageUnit));
            }
            else
            {
                for (Index i = 0; i < other.m_count; ++i)
                {
                    new (GetRawPtr(i)) TValueType(*other.GetPtr(i));
                }
            }
            m_count = other.m_count;
		}

		// Move constructor
		InplaceArray(InplaceArray&& other) noexcept(std::is_nothrow_move_constructible_v<TValueType>)
		{
			for (Index i = 0; i < other.m_count; ++i)
			{
				new (GetRawPtr(i)) TValueType(std::move(*other.GetPtr(i)));
			}
			m_count = other.m_count;
			other.Clear();
		}

		// Copy assignment operator
		InplaceArray& operator=(const InplaceArray& other) noexcept(
			std::is_nothrow_copy_constructible_v<TValueType> &&
			std::is_nothrow_copy_assignable_v<TValueType>)
		{
			if (this == std::addressof(other))
			{
				return *this;
			}

			// Assign to existing elements
			const Index minCount = (m_count < other.m_count) ? m_count : other.m_count;
			for (Index i = 0; i < minCount; ++i)
			{
				*GetPtr(i) = *other.GetPtr(i);
			}

			// Construct new elements if other has more
			for (Index i = minCount; i < other.m_count; ++i)
			{
				new (GetRawPtr(i)) TValueType(*other.GetPtr(i));
			}

			// Destroy excess elements if we had more
			if constexpr (!std::is_trivially_destructible_v<TValueType>)
			{
				for (Index i = other.m_count; i < m_count; ++i)
				{
					std::destroy_at(GetPtr(i));
				}
			}

			m_count = other.m_count;
			return *this;
		}

		// Move assignment operator
		InplaceArray& operator=(InplaceArray&& other) noexcept(
			std::is_nothrow_move_constructible_v<TValueType> &&
			std::is_nothrow_move_assignable_v<TValueType>)
		{
			if (this == std::addressof(other))
			{
				return *this;
			}

			// Assign to existing elements
			const Index minCount = (m_count < other.m_count) ? m_count : other.m_count;
			for (Index i = 0; i < minCount; ++i)
			{
				*GetPtr(i) = std::move(*other.GetPtr(i));
			}

			// Construct new elements if other has more
			for (Index i = minCount; i < other.m_count; ++i)
			{
				new (GetRawPtr(i)) TValueType(std::move(*other.GetPtr(i)));
			}

			// Destroy excess elements if we had more
			if constexpr (!std::is_trivially_destructible_v<TValueType>)
			{
				for (Index i = other.m_count; i < m_count; ++i)
				{
					std::destroy_at(GetPtr(i));
				}
			}

			m_count = other.m_count;
			other.Clear();
			return *this;
		}

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
				return *GetPtr(0); // Return first element as fallback (UB if empty, but we're in error state anyway)
			}

			new (GetRawPtr(m_count)) TValueType(std::forward<Args>(args)...);
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


		//access
		TValueType& operator[](const Index index) noexcept
		{
			VSB_ASSERT_V(index < m_count && index >= 0, "Index out of bounds", index);
			return *GetPtr(index);
		}


		const TValueType& operator[](const Index index) const noexcept
		{
			VSB_ASSERT_V(index < m_count && index >= 0, "Index out of bounds", index);
			return *GetPtr(index);
		}


		[[nodiscard]] TValueType Get(const Index index, const TValueType defaultValue = TValueType{}) const noexcept
		{
			if (index < m_count && index >= 0)
			{
				return *GetPtr(index);
			}

			return defaultValue;
		}


		void Set(const Index index, const TValueType value) noexcept
		{
			if (index < m_count && index >= 0)
			{
				*GetPtr(index) = value;
			}
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

		struct StorageUnit
		{
			alignas(TValueType) std::byte data[sizeof(TValueType)];
		};

		void* GetRawPtr(const Index index) noexcept
		{
			return std::addressof(m_data[index].data);
		}

		[[nodiscard]] const void* GetRawPtr(const Index index) const noexcept
		{
			return std::addressof(m_data[index].data);
		}


		TValueType* GetPtr(const Index index) noexcept
		{
			return reinterpret_cast<TValueType*>(std::addressof(m_data[index].data)); // NOLINT(*-pro-type-reinterpret-cast)
		}


		[[nodiscard]] const TValueType* GetPtr(const Index index) const noexcept
		{
			return reinterpret_cast<const TValueType*>(std::addressof(m_data[index].data)); // NOLINT(*-pro-type-reinterpret-cast)
		}


		StorageUnit m_data[Capacity];

		Count m_count = 0;
	};

	static_assert(std::is_trivially_copyable_v<InplaceArray<int, 5>> == false);

}