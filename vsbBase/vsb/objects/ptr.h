//(C) 2025 Alexander Samarin

#pragma once


#ifdef DEBUG
#define VSB_PTR_VALIDATE
#endif

#include "internal/handle.h"
#include "internal/object_registry.h"
#include "vsb/utils.h"
#include "vsb/debug.h"


#ifdef VSB_PTR_VALIDATE
#define PTR_HANDLE_CHECK VSB_ASSERT(internal::ObjectRegistry::GetInstance().ValidateHandle(m_handle), "");
#else
#define PTR_HANDLE_CHECK void(0);
#endif






namespace vsb
{
	template<typename T>
	class Ptr
	{
		template<typename U> friend class Ptr;

	public:

		Ptr() = default;
		~Ptr() = default;

		Ptr(const Ptr&) = default;
		Ptr(Ptr&&) = default;
		Ptr& operator=(const Ptr&) = default;
		Ptr& operator=(Ptr&&) = default;


		template<typename U>
		explicit(std::is_base_of_v<T, U> == false) Ptr(const Ptr<U>& copyFrom) : m_pointer(ExtractPointer(copyFrom.m_pointer))
		{
#ifdef VSB_PTR_VALIDATE
			if (m_pointer != nullptr)
			{
				m_handle = copyFrom.m_handle;
			}
#endif
		}

		template<typename U>
		explicit(std::is_base_of_v<T, U> == false) Ptr(Ptr<U>&& moveFrom) : m_pointer(ExtractPointer(moveFrom.m_pointer))
		{
			moveFrom.m_pointer = nullptr;

#ifdef VSB_PTR_VALIDATE
			if (m_pointer != nullptr)
			{
				m_handle = moveFrom.m_handle;
			}
#endif

			moveFrom.m_handle = {};
		}


		template<typename U>
		explicit(std::is_base_of_v<T, U> == false) Ptr(U* ptr) : m_pointer(ExtractPointer(ptr))
		{
#ifdef VSB_PTR_VALIDATE
			if (m_pointer != nullptr)
			{
				auto pBasePtr = dynamic_cast<Object*>(m_pointer);
				VSB_ASSERT(pBasePtr != nullptr, "Failed to downcast to Object");
				// ReSharper disable once CppDFANullDereference
				m_handle = pBasePtr->GetHandle();
			}
#endif
		}


		explicit operator bool() const
		{
#ifdef VSB_PTR_VALIDATE
			if (m_pointer != nullptr)
			{
				PTR_HANDLE_CHECK;
			}
#endif
			return m_pointer != nullptr;
		}


		[[nodiscard]] bool IsEmpty() const
		{
#ifdef VSB_PTR_VALIDATE
			if (m_pointer != nullptr)
			{
				PTR_HANDLE_CHECK;
			}
#endif
			return m_pointer == nullptr;
		}


		[[nodiscard]] bool IsValid() const
		{
#ifdef VSB_PTR_VALIDATE
			if (m_pointer != nullptr)
			{
				PTR_HANDLE_CHECK;
			}
#endif
			return m_pointer != nullptr;
		}


		T* operator->() const
		{
			VSB_ASSERT(m_pointer != nullptr, "pointer shouldn't be null if addressed via -> operator");
			PTR_HANDLE_CHECK;

			return m_pointer;
		}


		T* Get() const
		{
#ifdef VSB_PTR_VALIDATE
			if (m_pointer != nullptr)
			{
				PTR_HANDLE_CHECK;
			}
#endif

			return m_pointer;
		}


		T& GetRef() const
		{
			VSB_ASSERT(m_pointer != nullptr, "pointer shouldn't be null if addressed via GetRef()");
			PTR_HANDLE_CHECK;

			// ReSharper disable once CppDFANullDereference
			return *m_pointer;
		}


		void Reset()
		{
			m_pointer = nullptr;
#ifdef VSB_PTR_VALIDATE
			m_handle = internal::Handle::Empty;
#endif
		}

		template<typename U>
		bool operator==(const Ptr<U>& other) const
		{
            if constexpr (std::is_convertible_v<U*, T*>)
            {
                return m_pointer == static_cast<T*>(other.Get());
            }
            else if constexpr (std::is_base_of_v<Object, T> && std::is_base_of_v<Object, U>)
            {
                return m_pointer == other.Get(); // implicit to Object* if you want that route, but not required here
            }
            else
            {
                return m_pointer == dynamic_cast<const T*>(other.Get());
            }
		}

	private:


		template<typename U>
		T* ExtractPointer(U* pPointer)
		{
			return vsb::SafeCast<T>(pPointer);
		}


		T* m_pointer {nullptr};

#ifdef VSB_PTR_VALIDATE
		internal::Handle m_handle {};
#endif

	};

	template<typename T>
	Ptr<T> CreatePtr(T* pointer)
	{
		return Ptr<T>(pointer);
	}

	template<typename TPtrType, typename TInputPointerType>
	Ptr<TPtrType> CreatePtr(TInputPointerType* pointer)
	{
		return Ptr<TPtrType>(pointer);
	}

	static_assert(std::is_trivially_copyable_v<Ptr<Object>>);
	static_assert(std::is_standard_layout_v<Ptr<Object>>);
}


#ifdef VSB_PTR_VALIDATE
#undef VSB_PTR_VALIDATE
#endif

#undef PTR_HANDLE_CHECK