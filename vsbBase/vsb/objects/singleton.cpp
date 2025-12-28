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

		for (Index i = singletonList.GetSize() - 1; i >= 0; i--)
		{
			if (auto* pSingleton = singletonList[i].pSingleton.ValidateAndGet())
			{
				delete pSingleton; // NOLINT(*-owning-memory)
			}
			else
			{
				VSBLOG_ERROR("Singleton is null");
			}
		}

		singletonList.Clear();
	}


	void SingletonBase::RegisterSingleton(const SafePtr<SingletonBase> &singletonPtr, int priority)
	{
		auto& singletonList = GetSingletonList();
		singletonList.Add(SingletonEntry{ .pSingleton=singletonPtr, .priority=priority });
	}


	SingletonBase::SingletonList& SingletonBase::GetSingletonList()
	{
		static SingletonList singletonList;
		return singletonList;
	}
}
