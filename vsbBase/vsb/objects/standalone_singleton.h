//
// Created by Alexander on 12.03.2026.
//

#pragma once

#include <vsb/vsb_settings.h>
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
#ifdef VSB_SINGLE_THREAD_CHECK
				if (!s_MainThreadIDWasInit)
				{
					s_MainThreadID = std::this_thread::get_id();
					s_MainThreadIDWasInit = true;
				}
#endif
			}

			~StandaloneSingletonBase()
			{
				s_CurrentCount--;
			}

#ifdef VSB_SINGLE_THREAD_CHECK
			static void CheckThreadAccess()
			{
				const auto currentThread = std::this_thread::get_id();

				if (!s_MainThreadIDWasInit)
				{
					s_MainThreadID = currentThread;
					s_MainThreadIDWasInit = true;
					return;
				}

				const auto mainThread = s_MainThreadID;
				const bool sameThread = currentThread == mainThread;
				VSB_ASSERT(sameThread, "Singleton is not thread safe");
			}
#endif

		private:

#ifdef VSB_SINGLE_THREAD_CHECK
			inline static bool s_MainThreadIDWasInit = false;
			inline static std::thread::id s_MainThreadID;
#endif

			inline static int s_CurrentCount {0};
		};
	}


	template<typename TSingleton, bool publicInstanceAccess = true>
	class StandaloneSingleton : public internal::StandaloneSingletonBase
	{
	public:

		StandaloneSingleton(const StandaloneSingleton&) = delete;
		StandaloneSingleton& operator=(const StandaloneSingleton&) = delete;
		StandaloneSingleton(StandaloneSingleton&&) = delete;
		StandaloneSingleton& operator=(StandaloneSingleton&&) = delete;

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

	private:

		static TSingleton* GetInstanceImpl()
		{
			auto* instance = GetInstanceIfAvailableImpl();
			VSB_ASSERT(instance != nullptr, "Singleton is not initialized");
			return instance;
		}

		static TSingleton* GetInstanceIfAvailableImpl()
		{
			if (s_pInstance == nullptr && s_WasShutDown == false) [[unlikely]]
			{
				Instantiate();
			}

#ifdef VSB_SINGLE_THREAD_CHECK
			CheckThreadAccess();
#endif

			return s_pInstance;
		}

		static void Instantiate()
		{
			static TSingleton instance;
		}

		static constinit inline TSingleton* s_pInstance {nullptr};
		static constinit inline bool s_WasShutDown {false};
	};
}