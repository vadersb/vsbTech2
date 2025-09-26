//(C) 2025 Alexander Samarin

#pragma once
#include "internal/handle.h"
#include "internal/object_registry.h"
#include "vsb/hash.h"

namespace vsb
{
	template<typename T>
	struct Hash64Provider<Hnd<T>>;

	template<typename T>
	class Hnd
	{

	friend struct Hash64Provider<Hnd<T>>;

	public:

		Hnd()=default;
		~Hnd()=default;

		Hnd(const Hnd&) = default;
		Hnd(Hnd&&) = default;
		Hnd& operator=(const Hnd&) = default;
		Hnd& operator=(Hnd&&) = default;


		template<typename U>
		explicit(std::is_base_of_v<T, U> == false) Hnd(const Hnd<U>& other)
		: m_handle(ExtractFromTypedHandle<U>(other.m_handle))
		{}

		template<typename U>
		explicit(std::is_base_of_v<T, U> == false) Hnd(Hnd<U>&& moveFrom) 
		: m_handle(ExtractFromTypedHandle<U>(std::move(moveFrom.m_handle)))
{
}


		explicit Hnd(const internal::Handle &handle) : m_handle(ExtractFromHandle(handle))
		{
		}


		template<typename U>
		explicit Hnd(U* pPointer) : m_handle(ExtractFromPointer(pPointer))
		{
		}


		[[nodiscard]] bool IsEmpty() const {return m_handle.IsEmpty();}

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
			}

			return isValid;
		}


		[[nodiscard]] bool IsValid() const
		{
			if (m_handle.IsEmpty())
			{
				return false;
			}

			const bool isValid = internal::ObjectRegistry::GetInstance().ValidateHandle(m_handle);

			return isValid;
		}


		T* ValidateAndGet()
		{
			auto pTargetPtr = GetValidatedPointer();

			if (pTargetPtr == nullptr)
			{
				m_handle = internal::Handle::Empty;
			}

			return pTargetPtr;
		}


		[[nodiscard]] T* GetIfValid() const
		{
			return GetValidatedPointer();
		}


		void Reset()
		{
			m_handle = internal::Handle::Empty;
		}


	private:

		T* GetValidatedPointer() const
		{
			if (m_handle.IsEmpty()) return nullptr;
    
			auto pBasePtr = internal::ObjectRegistry::GetInstance().GetObject(m_handle);
			auto* pCastedPtr = static_cast<T*>(pBasePtr);  // Fast! Type already validated

#ifdef DEBUG  // or #ifndef NDEBUG
		    if (pBasePtr != nullptr)
		    {
		        auto* pDynamicCastedPointer = dynamic_cast<T*>(pBasePtr);
		        VSB_ASSERT(pDynamicCastedPointer == pCastedPtr, "static_cast and dynamic_cast results differ!");
			}
#endif

			return pCastedPtr;
		}


		template<typename U>
		static internal::Handle ExtractFromTypedHandle(internal::Handle handle)
		{
			if (handle.IsEmpty())
			{
				return handle;
			}

			if constexpr(std::same_as<T, U> || std::is_base_of_v<T, U>)
			{
				return handle;
			}

			const auto pBasePtr = internal::ObjectRegistry::GetInstance().GetObject(handle);
			const auto pTargetPtr = dynamic_cast<T*>(pBasePtr);

			return pTargetPtr == nullptr ? internal::Handle::Empty : handle;
		}


		static internal::Handle ExtractFromHandle(internal::Handle handle)
		{
			if (handle.IsEmpty())
			{
				return handle;
			}

			const auto pBasePtr = internal::ObjectRegistry::GetInstance().GetObject(handle);
			const auto pTargetPtr = dynamic_cast<T*>(pBasePtr);

			return pTargetPtr == nullptr ? internal::Handle::Empty : handle;
		}

		template<typename U>
		static internal::Handle ExtractFromPointer(U* pPointer)
		{
			if (pPointer == nullptr)
			{
				return internal::Handle::Empty;
			}

			T* pTargetPtr;

			if constexpr (std::same_as<T, U> || std::is_base_of_v<T, U>)
			{
				pTargetPtr = static_cast<T*>(pPointer);
			}
			else
			{
				pTargetPtr = dynamic_cast<T*>(pPointer);

				if (pTargetPtr == nullptr)
				{
					return internal::Handle::Empty;
				}
			}

			if constexpr (std::is_base_of_v<Object, T>)
			{
				return static_cast<Object*>(pTargetPtr)->GetHandle();
			}
			else if (std::is_base_of_v<Object, U>)
			{
				return static_cast<Object*>(pPointer)->GetHandle();
			}
			else
			{
				const auto pBasePtr = dynamic_cast<Object*>(pPointer);

				if (pBasePtr != nullptr)
				{
					return pBasePtr->GetHandle();
				}

				return internal::Handle::Empty;
			}
		}

		internal::Handle m_handle {};
	};

	static_assert(std::is_trivially_copyable_v<Hnd<Object>>);
	static_assert(std::is_standard_layout_v<Hnd<Object>>);



	template<typename T>
	Hnd<T> CreateHnd(T* pointer)
	{
		return Hnd<T>(pointer);
	}

	template<typename THndType, typename TPointerType>
	Hnd<THndType> CreateHnd(TPointerType* pointer)
	{
		return Hnd<THndType>(pointer);
	}

	template<typename T>
	struct Hash64Provider<Hnd<T>>
	{
		constexpr static Hash64 Get(const Hnd<T>& key) noexcept
		{
			return key.m_handle.GetHash64();
		}
	};
}