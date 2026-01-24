//(C) 2025 Alexander Samarin

#pragma once

#include "object.h"
#include "safe_ptr.h"


namespace vsb
{
	template<typename TSingleton, int priority>
	class Singleton;

	class SingletonBase : public Object
	{
		template<typename TSingleton, int priority>
		friend class Singleton;

	public:

		static void DestroyAllSingletons();


	protected:

		SingletonBase();

		~SingletonBase() override = default;

	private:

		static void RegisterSingleton(const SafePtr<SingletonBase> &singletonPtr, int priority = 0);

		struct SingletonEntry
		{
			SafePtr<SingletonBase> pSingleton;
			int priority;

			auto operator<=>(const SingletonEntry& other) const
			{
				return priority <=> other.priority;
			}
		};

		static constexpr size_t SingletonEntriesMaxCount = 1024;

		using SingletonList = std::pmr::vector<SingletonEntry>;

		static SingletonList& GetSingletonList();
	};


	template<typename TSingleton, int priority = 0>
	class Singleton : public SingletonBase
	{
	public:

		static TSingleton& GetInstance()
		{
			VSB_ASSERT(s_isInitialized, "Singleton is not initialized!");
			VSB_ASSERT(s_pInstance != nullptr, "Singleton instance is null!");
			return *s_pInstance;
		}


		static TSingleton* GetInstancePtrIfAvailable()
		{
			return s_pInstance;
		}


		static void Init()
		{
			if (s_isInitialized)
			{
				return;
			}

			s_pInstance = new TSingleton();
			s_isInitialized = true;
		}


	protected:

		Singleton()
		{
			RegisterSingleton(CreateSafePtr(this), priority);
		}


		~Singleton() override
		{
			s_pInstance = nullptr;
		}


	private:

		static inline TSingleton* s_pInstance {nullptr};
		static inline bool s_isInitialized {false};
	};
}
