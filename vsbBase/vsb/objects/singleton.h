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
			return GetInstanceSafePtr().GetRef();
		}


		static TSingleton* GetInstancePtrIfAvailable()
		{
			auto safePtrCopy = GetInstanceSafePtr();
			return safePtrCopy.ValidateAndGet();
		}


	protected:

		Singleton()
		{
			RegisterSingleton(CreateSafePtr(this), priority);
		}


	private:

		static SafePtr<TSingleton>& GetInstanceSafePtr()
		{
			static SafePtr<TSingleton> instance = new TSingleton();
			return instance;
		}
	};
}
