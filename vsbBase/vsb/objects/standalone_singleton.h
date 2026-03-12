//
// Created by Alexander on 12.03.2026.
//

#pragma once

#include "vsb/debug.h"

namespace vsb
{
	namespace internal
	{
		class StandaloneSingletonBase
		{
		public:

			static int GetCount() {return s_CurrentCount;}

		protected:

			StandaloneSingletonBase()
			{
				s_CurrentCount++;
			}

			~StandaloneSingletonBase()
			{
				s_CurrentCount--;
			}


		private:

			inline static int s_CurrentCount {0};
		};
	}


	template<typename TSingleton, bool publicInstanceAccess = true>
	class StandaloneSingleton : public internal::StandaloneSingletonBase
	{
	public:

		[[nodiscard]] static TSingleton* GetInstance() requires publicInstanceAccess
		{
			return GetInstanceImpl();
		}

		[[nodiscard]] static TSingleton* GetInstanceIfAvailable() requires publicInstanceAccess
		{
			return GetInstanceIfAvailableImpl();
		}

	protected:

		[[nodiscard]] static TSingleton* GetInstance() requires (!publicInstanceAccess)
		{
			return GetInstanceImpl();
		}

		[[nodiscard]] static TSingleton* GetInstanceIfAvailable() requires (!publicInstanceAccess)
		{
			return GetInstanceIfAvailableImpl();
		}


		StandaloneSingleton()
		{
			VSB_ASSERT(s_pInstance == nullptr, "Singleton already exists");
			VSB_ASSERT(s_WasShutDown == false, "Singleton re-creation");
			s_pInstance = static_cast<TSingleton*>(this);
		}

		~StandaloneSingleton()
		{
			s_pInstance = nullptr;
			s_WasShutDown = true;
		}

		StandaloneSingleton(const StandaloneSingleton&) = delete;
		StandaloneSingleton& operator=(const StandaloneSingleton&) = delete;
		StandaloneSingleton(StandaloneSingleton&&) = delete;
		StandaloneSingleton& operator=(StandaloneSingleton&&) = delete;

	private:

		static TSingleton* GetInstanceImpl()
		{
			auto* instance = GetInstanceIfAvailableImpl();
			VSB_ASSERT(instance != nullptr, "Singleton is not initialized");
			return instance;
		}

		static TSingleton* GetInstanceIfAvailableImpl()
		{
			if (s_pInstance == nullptr && s_WasShutDown == false)
			{
				Instantiate();
			}

			return s_pInstance;
		}

		static void Instantiate()
		{
			static TSingleton instance;
		}


		static inline TSingleton* s_pInstance {nullptr};
		static inline bool s_WasShutDown {false};
	};
}