//(C) 2025 Alexander Samarin

#pragma once

#include "object.h"
#include "safe_ptr.h"
#include "vsb/containers/inplace_array.h"


namespace vsb
{
	class SingletonBase : public Object
	{
	public:

		static void DestroyAllSingletons();

	protected:

		SingletonBase();

		~SingletonBase() override = default;


		static void RegisterSingleton(const SafePtr<SingletonBase> &singletonPtr, int priority = 0);


	private:

		struct SingletonEntry
		{
			SafePtr<SingletonBase> pSingleton;
			int priority;
		};

		static constexpr Count SingletonEntriesMaxCount = 1024;

		using SingletonList = InplaceArray<SingletonEntry, SingletonEntriesMaxCount>;

		static SingletonList& GetSingletonList();

	};


	template<typename TSingleton, int priority = 0>
	class Singleton : public SingletonBase
	{
	public:

		static TSingleton& GetInstance()
		{
			static SafePtr<TSingleton> instance = new TSingleton();
			return instance.GetRef();
		}

	protected:

		Singleton()
		{
			RegisterSingleton(CreateSafePtr(this), priority);
		}


	};
}
