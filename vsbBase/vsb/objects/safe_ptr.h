//(C) 2025 Alexander Samarin

#pragma once

#include "internal/handle.h"
#include "internal/object_registry.h"


namespace vsb
{
	template <typename T>
	class SafePtr
	{

		template<typename U> friend class SafePtr;


	public:

		SafePtr() = default;
		~SafePtr() = default;

		SafePtr(const SafePtr&) = default;
		SafePtr(SafePtr&&) = default;
		SafePtr& operator=(const SafePtr&) = default;
		SafePtr& operator=(SafePtr&&) = default;


		//constructing from raw pointer
		template<typename U>
		explicit(std::is_base_of_v<T, U> == false) SafePtr(U* pointer) :
			m_pointer(ExtractPointer(pointer)),
			m_handle(ExtractHandle(m_pointer))
		{
			if (m_pointer == nullptr && m_handle.IsEmpty() == false)
			{
				VSBLOG_ERROR("SafePtr pointer is null, while handle isn't");
				m_handle = internal::Handle::Empty;
				return;
			}

			if (m_pointer != nullptr && m_handle.IsEmpty())
			{
				VSBLOG_ERROR("SafePtr pointer isn't null, while handle is empty");
				m_pointer = nullptr;
			}
		}

		template<typename U>
		explicit(std::is_base_of_v<T, U> == false) SafePtr(const SafePtr<U>& other) :
		    m_pointer(ExtractPointer(other.m_pointer)),
		    m_handle(other.m_handle)
		{
		    // Validate the cast worked
		    if (other.m_pointer != nullptr && m_pointer == nullptr)
		    {
		    	VSBLOG_ERROR("other pointer isn't null, while extracted one is");
		        m_handle = internal::Handle::Empty;
		    }
		}

		template<typename U>
		explicit(std::is_base_of_v<T, U> == false) SafePtr(SafePtr<U>&& other) :
		    m_pointer(ExtractPointer(other.m_pointer)),
		    m_handle(std::move(other.m_handle))
		{
			// Clear the source first
			auto* temp_ptr = other.m_pointer;  // Store before clearing
			other.m_pointer = nullptr;
			other.m_handle = internal::Handle::Empty;

			// Then validate the cast worked
			if (temp_ptr != nullptr && m_pointer == nullptr)
			{
				VSBLOG_ERROR("other pointer isn't null, while extracted one is");
				m_handle = internal::Handle::Empty;
			}

		}



		explicit operator bool() const
		{
			return m_pointer != nullptr;
		}


		[[nodiscard]] bool IsEmpty() const
		{
			return m_pointer == nullptr;
		}


		[[nodiscard]] bool IsValid() const
		{
			if (m_pointer == nullptr)
			{
				return false;
			}

			return internal::ObjectRegistry::GetInstance().ValidateHandle(m_handle);
		}


		bool Validate()
		{
			if (m_handle.IsEmpty())
			{
				return false;
			}

			const bool isValid = internal::ObjectRegistry::GetInstance().ValidateHandle(m_handle);

			if (!isValid)
			{
				m_handle = internal::Handle::Empty;
				m_pointer = nullptr;
			}

			return isValid;
		}


		T* ValidateAndGet()
		{
			if (m_handle.IsEmpty())
			{
				return nullptr;
			}

			const bool isValid = internal::ObjectRegistry::GetInstance().ValidateHandle(m_handle);

			if (!isValid)
			{
				m_handle = internal::Handle::Empty;
				m_pointer = nullptr;
			}

			return m_pointer;
		}


		T* operator->() const
		{
			VSB_ASSERT(m_pointer != nullptr, "pointer shouldn't be null if addressed via -> operator");

#ifdef DEBUG
			const bool isValid = internal::ObjectRegistry::GetInstance().ValidateHandle(m_handle);
			VSB_ASSERT(isValid, "SafePtr pointer is expired");
#endif

			return m_pointer;
		}


		T* Get() const
		{
#ifdef DEBUG
			if (m_pointer != nullptr)
			{
				const bool isValid = internal::ObjectRegistry::GetInstance().ValidateHandle(m_handle);
				VSB_ASSERT(isValid, "SafePtr pointer is expired");
			}
#endif

			return m_pointer;
		}


		T& GetRef() const
		{
			VSB_ASSERT(m_pointer != nullptr, "SafePtr pointer is null");
#ifdef DEBUG
			const bool isValid = internal::ObjectRegistry::GetInstance().ValidateHandle(m_handle);
			VSB_ASSERT(isValid, "SafePtr pointer is expired");
#endif
			// ReSharper disable once CppDFANullDereference
			return *m_pointer;
		}


		void Reset()
		{
			m_pointer = nullptr;
			m_handle = internal::Handle::Empty;
		}


	private:


		static internal::Handle ExtractHandle(T* pointer)
		{
			if (pointer == nullptr)
			{
				return internal::Handle::Empty;
			}

			if constexpr (std::is_base_of_v<Object, T>)
			{
				const auto pBasePtr = static_cast<Object*>(pointer);
				return pBasePtr->GetHandle();
			}

			const auto pBasePtr = dynamic_cast<Object*>(pointer);
			return pBasePtr == nullptr ? internal::Handle::Empty : pBasePtr->GetHandle();
		}


		template<typename U>
		static T* ExtractPointer(U* pointer)
		{
			if (pointer == nullptr)
			{
				return nullptr;
			}

			if constexpr (std::is_same_v<T, U>)
			{
				return pointer;
			}
			// else if (std::is_base_of_v<T, U>)
			// {
			// 	return static_cast<T*>(pointer);
			// }

			return dynamic_cast<T*>(pointer);
		}



		T* m_pointer {nullptr};
		internal::Handle m_handle {};
	};


	template<typename T>
	SafePtr<T> CreateSafePtr(T* pointer)
	{
		return SafePtr<T>(pointer);
	}

	template<typename TPtrType, typename TInputPointerType>
	SafePtr<TPtrType> CreateSafePtr(TInputPointerType* pointer)
	{
		return SafePtr<TPtrType>(pointer);
	}
}