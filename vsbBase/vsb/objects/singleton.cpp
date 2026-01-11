//(C) 2025 Alexander Samarin

#include "singleton.h"

#include "vsb/log.h"
#include "vsb/containers/container_utils.h"

namespace vsb
{
	SingletonBase::SingletonBase(): Object(ObjectHint::Singleton)
	{}


	void SingletonBase::DestroyAllSingletons()
	{
		auto& singletonList = GetSingletonList();

		std::stable_sort(singletonList.begin(), singletonList.end(), std::greater{});

		for (auto& entry : singletonList)
		{
			if (auto* pSingleton = entry.pSingleton.ValidateAndGet()) // NOLINT(*-delete-null-pointer)
			{
				delete pSingleton;
			}
			else
			{
				VSBLOG_ERROR("Singleton is null");
			}
		}

		singletonList.clear();
	}


	void SingletonBase::RegisterSingleton(const SafePtr<SingletonBase> &singletonPtr, int priority)
	{
		auto& singletonList = GetSingletonList();
		singletonList.push_back(SingletonEntry{ .pSingleton=singletonPtr, .priority=priority });
	}


	SingletonBase::SingletonList& SingletonBase::GetSingletonList()
	{
		static TempVectorSetup<SingletonEntry, 2048> singletonListSetup;
		return singletonListSetup.GetVector();
	}
}
