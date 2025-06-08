//(C) 2025 Alexander Samarin

#include "singleton.h"

namespace vsb
{
	SingletonBase::SingletonBase(): Object(ObjectHint::Singleton)
	{}


	void SingletonBase::DestroyAllSingletons()
	{
		auto& singletonList = GetSingletonList();

		//todo later: sort by priority

		for (auto& singletonEntry : singletonList)
		{
			if (auto* pSingleton = singletonEntry.pSingleton.ValidateAndGet())
			{
				delete pSingleton;
			}
			else
			{
				VSBLOG_WARN("Singleton is null");
			}
		}

		singletonList.Clear();
	}





	void SingletonBase::RegisterSingleton(const SafePtr<SingletonBase> &singletonPtr, int priority)
	{
		auto& singletonList = GetSingletonList();
		singletonList.Add(SingletonEntry{ singletonPtr, priority });
	}


	SingletonBase::SingletonList& SingletonBase::GetSingletonList()
	{
		static SingletonList singletonList;
		return singletonList;
	}
}
