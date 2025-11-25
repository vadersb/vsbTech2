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
		TValueType& operator[](const IndexEx index) noexcept
		{
			auto localIndex = index.GetIndex(Size);

			VSB_ASSERT(localIndex >= 0 && localIndex < Size, "Index out of bounds");

			return m_data[localIndex];
		}


		const TValueType& operator[](const IndexEx index) const noexcept
		{
			auto localIndex = index.GetIndex(Size);

			VSB_ASSERT(localIndex >= 0 && localIndex < Size, "Index out of bounds");

			return m_data[localIndex];
		}


		[[nodiscard]] TValueType Get(const IndexEx index, const TValueType defaultValue = TValueType{}) const noexcept
		{
			bool isValid;
			auto localIndex = index.GetIndex(Size, isValid);

			if (isValid)
			{
				return m_data[localIndex];
			}

			return defaultValue;
		}


		void Set(const IndexEx index, const TValueType value) noexcept
		{
			bool isValid;
			auto localIndex = index.GetIndex(Size, isValid);

			if (isValid)
			{
				m_data[localIndex] = value;
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

	static_assert(std::is_trivially_copyable_v<FixedArray<int, 5>>);
}