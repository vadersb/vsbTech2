//(C) 2025 Alexander Samarin

#pragma once
#include <algorithm>
#include <cstddef>
#include <cstring>
#include <memory>
#include <new>
#include <type_traits>
#include <concepts>
#include "objects/hnd.h"
#include "vsb/hash.h"

namespace vsb
{
	namespace internal
	{
		// Helper concept to check if a callable can be invoked with given arguments
		// This is more flexible than std::is_invocable_r_v for generic lambdas
		template<typename TCallable, typename TReturnType, typename... TArgs>
		concept InvocableWithResult = requires(TCallable&& callable, TArgs&&... args) {
			{ std::forward<TCallable>(callable)(std::forward<TArgs>(args)...) } -> std::convertible_to<TReturnType>;
		} || (std::is_void_v<TReturnType> && requires(TCallable&& callable, TArgs&&... args) {
			std::forward<TCallable>(callable)(std::forward<TArgs>(args)...);
		});
	}


	template<typename TReturnType, typename... TArgs>
	class Delegate
	{
	public:

		using FunctionPtr = TReturnType(*)(TArgs...);


		Delegate() = default;


		explicit Delegate(FunctionPtr pFunctionPtr)
		{
			if (pFunctionPtr)
			{
				auto rawPtr = GetDataRawPtr();
				new (rawPtr) FreeStandingFunctionCaller(pFunctionPtr);
			}
		}


		// Lambda/callable constructor - now supports generic lambdas with auto parameters
		template<typename TCallable>
		requires (!std::is_same_v<std::decay_t<TCallable>, Delegate> && 
		          !std::is_same_v<std::decay_t<TCallable>, FunctionPtr> &&
		          !std::is_pointer_v<std::decay_t<TCallable>> &&
				   internal::InvocableWithResult<TCallable, TReturnType, TArgs...>)
		explicit Delegate(TCallable&& callable)
		{
			using DecayedCallable = std::decay_t<TCallable>;
			static_assert(sizeof(LambdaCaller<DecayedCallable>) <= sizeof(uint64_t) * 4, 
			              "Lambda is too large - must fit in 32 bytes (sizeof(uint64_t) * 4)");
			
			auto rawPtr = GetDataRawPtr();
			new (rawPtr) LambdaCaller<DecayedCallable>(std::forward<TCallable>(callable));
		}


		template<typename TObject>
		Delegate(TObject* pObject, TReturnType(TObject::*pMemberFunction)(TArgs...))
		{
			if (pObject && pMemberFunction)
			{
				auto rawPtr = GetDataRawPtr();
				new (rawPtr) MemberFunctionCaller<TObject>(pObject, pMemberFunction);
			}
		}


		template<typename TObject>
		Delegate(Hnd<TObject> handle, TReturnType(TObject::*pMemberFunction)(TArgs...))
		{
			if (handle.IsEmpty() == false && pMemberFunction)
			{
				auto rawPtr = GetDataRawPtr();
				new (rawPtr) HandleFunctionCaller<TObject>(handle, pMemberFunction);
			}
		}


		Delegate(const Delegate& other)
		{
			if (!other.IsDataEmpty())
			{
				auto otherPtr = const_cast<Delegate&>(other).GetCallerBasePtr();
				CopyConstruct(otherPtr);
			}
		}


		Delegate(Delegate&& other) noexcept
		{
			if (!other.IsDataEmpty())
			{
				auto otherPtr = other.GetCallerBasePtr();
				MoveConstruct(otherPtr);
				other.Reset();
			}
		}


		Delegate& operator=(const Delegate& other)
		{
			if (this != &other)
			{
				Reset();
				if (!other.IsDataEmpty())
				{
					auto otherPtr = const_cast<Delegate&>(other).GetCallerBasePtr();
					CopyConstruct(otherPtr);
				}
			}
			return *this;
		}

		Delegate& operator=(Delegate&& other) noexcept
		{
			if (this != &other)
			{
				Reset();
				if (!other.IsDataEmpty())
				{
					auto otherPtr = other.GetCallerBasePtr();
					MoveConstruct(otherPtr);
					other.Reset();
				}
			}
			return *this;
		}


		// Assignment from a function pointer
		Delegate& operator=(FunctionPtr pFunctionPtr)
		{
			Reset();
			if (pFunctionPtr)
			{
				auto rawPtr = GetDataRawPtr();
				new (rawPtr) FreeStandingFunctionCaller(pFunctionPtr);
			}
			return *this;
		}


		// Assignment from lambda/callable - now supports generic lambdas
		template<typename TCallable>
		requires (!std::is_same_v<std::decay_t<TCallable>, Delegate> && 
		          !std::is_same_v<std::decay_t<TCallable>, FunctionPtr> &&
		          !std::is_pointer_v<std::decay_t<TCallable>> &&
	               internal::InvocableWithResult<TCallable, TReturnType, TArgs...>)
		Delegate& operator=(TCallable&& callable)
		{
			using DecayedCallable = std::decay_t<TCallable>;
			static_assert(sizeof(LambdaCaller<DecayedCallable>) <= sizeof(uint64_t) * 4, 
			              "Lambda is too large - must fit in 32 bytes (sizeof(uint64_t) * 4)");
			
			Reset();
			auto rawPtr = GetDataRawPtr();
			new (rawPtr) LambdaCaller<DecayedCallable>(std::forward<TCallable>(callable));
			return *this;
		}


		~Delegate()
		{
			Reset();
		}


		void Reset()
		{
			if (IsDataEmpty() == false)
			{
				auto ptr = GetCallerBasePtr();
				std::destroy_at(ptr);
				ResetData();
			}
		}



		TReturnType operator()(TArgs... args)
		{
			if (IsDataEmpty())
			{
                if constexpr (!std::is_void_v<TReturnType>)
                {
                	return TReturnType{};
                }
                else
                {
	                return;
                }
			}

			auto ptr = GetCallerBasePtr();
            
			if (ptr->IsExpired())
			{
				Reset();
                if constexpr (!std::is_void_v<TReturnType>)
                    return TReturnType{};
                else
                    return;
			}
            
			return ptr->Call(std::forward<TArgs>(args)...);
		}


		explicit operator bool() const noexcept
		{
			if (!IsDataEmpty())
			{
				return GetCallerBasePtr()->IsExpired() == false;
			}

			return false;
		}


		explicit operator bool() noexcept
		{
			if (!IsDataEmpty())
			{
				return GetCallerBasePtr()->IsExpired() == false;
			}

			return false;
		}


		bool operator==(const Delegate& other) const noexcept
		{
			if (&other == this)
			{
				return true;
			}

			const bool isDataEmpty = IsDataEmpty();

			if (isDataEmpty != other.IsDataEmpty())
			{
				return false;
			}

			if (isDataEmpty)
			{
				return true; // Both empty
			}

			auto thisPtr = GetConstCallerBasePtr();
			auto otherPtr = other.GetConstCallerBasePtr();
			
			// Compare type info first, then hash
			return typeid(*thisPtr) == typeid(*otherPtr) && thisPtr->GetHash() == otherPtr->GetHash();
		}

		bool operator!=(const Delegate& other) const noexcept
		{
			return !(*this == other);
		}


		bool operator==(std::nullptr_t) const noexcept
		{
			return IsDataEmpty();
		}

		
		bool operator!=(std::nullptr_t) const noexcept
		{
			return !IsDataEmpty();
		}



	private:



		class Caller
		{
		public:

			Caller() = default;
			virtual ~Caller() = default;

			[[nodiscard]] virtual bool IsExpired() const = 0;
			virtual bool IsExpired() = 0;

			virtual TReturnType Call(TArgs... args) = 0;

			virtual void CopyConstruct(void* pDest) const = 0;
			virtual void MoveConstruct(void* pDest) = 0;

			[[nodiscard]] virtual Hash64 GetHash() const = 0;

		private:


		};


		class FreeStandingFunctionCaller final : public Caller
		{
		public:

			explicit FreeStandingFunctionCaller(const FunctionPtr functionPtr) : m_functionPtr(functionPtr)
			{}


			[[nodiscard]] bool IsExpired() const override {return false;}
			bool IsExpired() override {return false;}

			TReturnType Call(TArgs... args) override
			{
				return m_functionPtr(std::forward<TArgs>(args)...);
			}


			void CopyConstruct(void* pDest) const override
			{
				new (pDest) FreeStandingFunctionCaller(*this);
			}

			void MoveConstruct(void* pDest) override
			{
				new (pDest) FreeStandingFunctionCaller(std::move(*this));
			}


			[[nodiscard]] Hash64 GetHash() const override
			{
				static_assert(sizeof(Hash64) == sizeof(FunctionPtr));
				return std::bit_cast<Hash64>(m_functionPtr);
			}

		private:

			FunctionPtr m_functionPtr = nullptr;
		};


		template<typename TCallable>
		class LambdaCaller final : public Caller
		{
		public:

			template<typename TForwardedCallable>
			requires (!std::is_same_v<std::decay_t<TForwardedCallable>, LambdaCaller>)
			explicit LambdaCaller(TForwardedCallable&& callable) 
				: m_callable(std::forward<TForwardedCallable>(callable))
			{}

			[[nodiscard]] bool IsExpired() const override { return false; }
			bool IsExpired() override { return false; }

			TReturnType Call(TArgs... args) override
			{
				// The magic happens here - auto parameters get deduced at call time
				if constexpr (std::is_void_v<TReturnType>)
				{
					m_callable(std::forward<TArgs>(args)...);
					return;
				}
				else
				{
					return m_callable(std::forward<TArgs>(args)...);
				}
			}

			void CopyConstruct(void* pDest) const override
			{
				new (pDest) LambdaCaller(*this);
			}

			void MoveConstruct(void* pDest) override
			{
				new (pDest) LambdaCaller(std::move(*this));
			}

			[[nodiscard]] Hash64 GetHash() const override
			{
				// For lambdas, we'll use the type hash combined with memory content hash
				// This provides a reasonable hash for comparison purposes
				const auto typeHash = typeid(TCallable).hash_code();
				const auto contentHash = CalculateHash64(m_callable);
				return static_cast<Hash64>(typeHash) ^ (contentHash << 1);
			}

		private:
			TCallable m_callable;
		};


		template<typename TObject>
		class MemberFunctionCaller final : public Caller
		{
		public:

			MemberFunctionCaller(TObject* pObject, TReturnType(TObject::*pMemberFunction)(TArgs...)) : m_pObject(pObject), m_pMemberFunction(pMemberFunction)
			{}

			[[nodiscard]] bool IsExpired() const override {return false;}
			bool IsExpired() override {return false;}

			TReturnType Call(TArgs... args) override
			{
				VSB_ASSERT(m_pObject != nullptr, "Object pointer is null");
				return (m_pObject->*m_pMemberFunction)(std::forward<TArgs>(args)...);
			}


			void CopyConstruct(void* pDest) const override
			{
				new (pDest) MemberFunctionCaller(*this);
			}

			void MoveConstruct(void* pDest) override
			{
				new (pDest) MemberFunctionCaller(std::move(*this));
			}


			[[nodiscard]] Hash64 GetHash() const override
			{
				static_assert(sizeof(Hash64) == sizeof(TObject*));
				const auto h1 = std::bit_cast<Hash64>(m_pObject);
				const auto h2 = GetMemberFunctionPointerHash(m_pMemberFunction);
				return h1 ^ (h2 << 1);
			}

		private:

			TObject* m_pObject = nullptr;
			TReturnType (TObject::*m_pMemberFunction)(TArgs...) {};
		};


		template<typename TObject>
		class HandleFunctionCaller final : public Caller
		{
		public:

			HandleFunctionCaller(Hnd<TObject> handle, TReturnType(TObject::*pMemberFunction)(TArgs...)) : m_handle(handle), m_pMemberFunction(pMemberFunction)
			{}


			[[nodiscard]] bool IsExpired() const override
			{
				return !m_handle.IsValid();
			}

			bool IsExpired() override
			{
				return !m_handle.Validate();
			}


			TReturnType Call(TArgs... args) override
			{
				auto pObject = m_handle.ValidateAndGet();
				VSB_ASSERT(pObject != nullptr, "Handle is invalid");

				return (pObject->*m_pMemberFunction)(std::forward<TArgs>(args)...);
			}


			void CopyConstruct(void* pDest) const override
			{
				new (pDest) HandleFunctionCaller(*this);
			}

			void MoveConstruct(void* pDest) override
			{
				new (pDest) HandleFunctionCaller(std::move(*this));
			}



			[[nodiscard]] Hash64 GetHash() const override
			{
				//static_assert(sizeof(Hash64) == sizeof(decltype(m_pMemberFunction)));
				const auto h1 = GetHash64(m_handle);
				const auto h2 = GetMemberFunctionPointerHash(m_pMemberFunction);
				return h1 ^ (h2 << 1);
			}


		private:

			Hnd<TObject> m_handle;
			TReturnType (TObject::*m_pMemberFunction)(TArgs...) {};
		};


		void CopyConstruct(const Caller* source)
		{
			source->CopyConstruct(GetDataRawPtr());
		}

		void MoveConstruct(Caller* source)
		{
			source->MoveConstruct(GetDataRawPtr());
		}


		template<typename TMemberFunctionPointer>
		static Hash64 GetMemberFunctionPointerHash(TMemberFunctionPointer pointer)
		{
            static_assert(std::is_member_function_pointer_v<TMemberFunctionPointer>);

            if constexpr (sizeof(TMemberFunctionPointer) == sizeof(Hash64))
            {
                return std::bit_cast<Hash64>(pointer);
            }
            else if constexpr (sizeof(TMemberFunctionPointer) == sizeof(Hash64) * 2)
            {
                auto hashArray = std::bit_cast<std::array<Hash64, 2>>(pointer);
                return hashArray[0] ^ (hashArray[1] << 1);
            }
            else if constexpr (sizeof(TMemberFunctionPointer) == sizeof(Hash64) * 3)
            {
                auto hashArray = std::bit_cast<std::array<Hash64, 3>>(pointer);
                return hashArray[0] ^ (hashArray[1] << 1) ^ (hashArray[2] << 2);
            }
            else
            {
                auto byteArray = std::bit_cast<std::array<std::byte, sizeof(TMemberFunctionPointer)>>(pointer);
                return CalculateHash64(byteArray);
            }
		}


		[[nodiscard]] bool IsDataEmpty() const
		{
			return std::memcmp(m_data, EmptyData, DataSize) == 0;
		}


		void ResetData()
		{
			std::memset(m_data, 0, DataSize);
		}


		void* GetDataRawPtr()
		{
			return m_data;
		}

		Caller* GetCallerBasePtr()
		{
			return std::launder(reinterpret_cast<Caller*>(m_data));
		}

		const Caller* GetConstCallerBasePtr() const
		{
			return std::launder(reinterpret_cast<const Caller*>(m_data));
		}

		//data size calculation
		class SampleClass : public Object
		{

		};


		static consteval size_t GetRequiredDataSize()
		{
            size_t result = 0;

            auto updateResult = [&result](const size_t size)
			{
                result = std::max(size, result);
            };

            updateResult(sizeof(FreeStandingFunctionCaller));
            updateResult(sizeof(MemberFunctionCaller<SampleClass>));
            updateResult(sizeof(HandleFunctionCaller<SampleClass>));

			// Ensure we have at least 32 bytes for lambdas
			updateResult(sizeof(uint64_t) * 4);

            return result;
		}


		static constexpr size_t DataSize = GetRequiredDataSize();
		static constexpr std::byte EmptyData[DataSize] = {};

		alignas(std::max_align_t) std::byte m_data[DataSize] = {};
	};
}