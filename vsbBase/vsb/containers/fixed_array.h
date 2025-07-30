//(C) 2025 Alexander Samarin

#pragma once
#include "container_defs.h"
#include "vsb/debug.h"


namespace vsb
{
	template<typename TValueType, Count Size>
	requires (Size >= 0)
	class FixedArray
	{
	public:

		// Default constructor
		FixedArray() = default;
		~FixedArray() = default;

		// Constructor with an initializer list
		FixedArray(std::initializer_list<TValueType> init)
		{
			if (init.size() > Size)
			{
				VSBLOG_WARN("Initializer list size is too large: {}, fixed array size: {}", init.size(), Size);
			}

		    Index i = 0;
		    for (const auto& value : init)
		    {
		    	if (i >= Size)
		    	{
		    		break;
		    	}

		        m_data[i++] = value;
		    }
		    
		    // Default-initialize remaining elements if any
		    for (; i < Size; ++i)
		    {
		        m_data[i] = TValueType{};
		    }
		}

		template<typename U>
		requires std::convertible_to<U, TValueType>
		explicit FixedArray(const U (&arr)[Size])
		{
		    std::copy(std::begin(arr), std::end(arr), begin());
		}

		// Seems like there is no need for this constructor
		// template<typename... Args>
		// requires (sizeof...(Args) <= Size) && (std::convertible_to<Args, TValueType> && ...)
		// explicit FixedArray(Args&&... args) : m_data{static_cast<TValueType>(std::forward<Args>(args))...}
		// {
		//     // Initialize remaining elements to default if Size > sizeof...(Args)
		//     if constexpr (sizeof...(Args) < Size)
		//     {
		//         for (Index i = sizeof...(Args); i < Size; ++i)
		//         {
		//             m_data[i] = TValueType{};
		//         }
		//     }
		// }


		//access
		TValueType& operator[](const Index index) noexcept
		{
			VSB_ASSERT_V(index < Size && index >= 0, "Index out of bounds", index);
			return m_data[index];
		}


		TValueType& operator[](const FromEnd offsetFromEnd) noexcept
		{
			auto index = offsetFromEnd.GetIndex(Size);
			VSB_ASSERT_V(index < Size && index >= 0, "Index out of bounds", index);
			return m_data[index];
		}


		const TValueType& operator[](const Index index) const noexcept
		{
			VSB_ASSERT_V(index < Size && index >= 0, "Index out of bounds", index);
			return m_data[index];
		}


		const TValueType& operator[](const FromEnd offsetFromEnd) const noexcept
		{
			auto index = offsetFromEnd.GetIndex(Size);
			VSB_ASSERT_V(index < Size && index >= 0, "Index out of bounds", index);
			return m_data[index];
		}


		TValueType Get(const Index index, const TValueType defaultValue = TValueType{}) const noexcept
		{
			if (index < Size && index >= 0)
			{
				return m_data[index];
			}

			return defaultValue;
		}


		TValueType Get(const FromEnd offsetFromEnd, const TValueType defaultValue = TValueType{}) const noexcept
		{
			auto index = offsetFromEnd.GetIndex(Size);
			if (index < Size && index >= 0)
			{
				return m_data[index];
			}

			return defaultValue;
		}


		void Set(const Index index, const TValueType value) noexcept
		{
			if (index < Size && index >= 0)
			{
				m_data[index] = value;
			}
		}


		void Set(const FromEnd offsetFromEnd, const TValueType value) noexcept
		{
			auto index = offsetFromEnd.GetIndex(Size);
			if (index < Size && index >= 0)
			{
				m_data[index] = value;
			}
		}


		//modifications
		void Fill(const TValueType value = TValueType{}) noexcept
		{
			for (Index i = 0; i < Size; ++i)
			{
				m_data[i] = value;
			}
		}



		//queries
		[[nodiscard]] static constexpr bool IsEmpty() noexcept {return Size == 0;}
		[[nodiscard]] static constexpr Count GetSize() noexcept {return Size;}


		// Iterators for compatibility with STL algorithms
		[[nodiscard]] TValueType* begin() noexcept 
		{ 
		    if constexpr (Size == 0) return nullptr;
		    return m_data; 
		}
		[[nodiscard]] const TValueType* begin() const noexcept { return m_data; }
		[[nodiscard]] const TValueType* cbegin() const noexcept { return m_data; }

		[[nodiscard]] TValueType* end() noexcept { return m_data + Size; }
		[[nodiscard]] const TValueType* end() const noexcept { return m_data + Size; }
		[[nodiscard]] const TValueType* cend() const noexcept { return m_data + Size; }


	private:
		
		std::conditional_t<Size == 0, char, TValueType[Size]> m_data;
	};

	template<typename T, typename... U>
	FixedArray(T, U...) -> FixedArray<T, 1 + sizeof...(U)>;
}