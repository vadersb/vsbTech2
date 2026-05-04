//
// Created by Alexander on 17.01.2026.
//

#pragma once

#include <cstdint>
#include <vector>
#include <stack>
#include <array>
#include <algorithm>
#include "vsb/log.h"
#include "vsb/utils.h"
#include "vsb/vsb_settings.h"
#include "vsb/debug.h"



namespace vsb::memory
{
	class SingleThreadedPool
	{
	public:

		static void Init()
		{
			for (int i = 0; i < BucketsCount; i++)
			{
				TemplateIndexedDispatch<BucketsCount>(i, []<std::size_t index>()
				{
					Bucket<index>::Init();
				});
			}

			s_WasInit = true;
		}

		static std::int64_t GetObjectsCount() {return s_ObjectsCount;}

		static void* Allocate(const size_t size)
		{
			VSB_CHECK_THREAD();
			VSB_ASSERT(s_WasInit, "Pool was not initialized!");

			const auto bucketIndex = GetBucketIndex(size);

			if (bucketIndex == -1)
			{
				return std::malloc(size);
			}

			void* pResult;

			TemplateIndexedDispatch<BucketsCount>(bucketIndex, [&pResult]<std::size_t index>()
			{
				pResult = Bucket<index>::GetInstance()->Allocate();
			});

			s_ObjectsCount++;
			return pResult;
		}


		static void Deallocate(void* pElement, const size_t size)
		{
			VSB_CHECK_THREAD();
			VSB_ASSERT(s_WasInit, "Pool was not initialized!");

			const auto bucketIndex = GetBucketIndex(size);

			if (bucketIndex == -1)
			{
				std::free(pElement);
				return;
			}

			TemplateIndexedDispatch<BucketsCount>(bucketIndex, [pElement]<std::size_t index>()
			{
				if (!Bucket<index>::WasShutDown())
				{
					Bucket<index>::GetInstance()->Deallocate(pElement);
				}
			});

			s_ObjectsCount--;
		}

	private:

		struct BucketSettings
		{
			size_t elementSize;
			size_t elementsCount;
		};

		static constexpr std::array s_BucketSettings {
			BucketSettings {8ul, 64ul * 1024},
			BucketSettings {16ul, 64ul * 1024},
			BucketSettings {32ul, 64ul * 1024},
			BucketSettings {64ul, 32ul * 1024},
			BucketSettings {128ul, 16ul * 1024},
			BucketSettings {256ul, 8ul * 1024},
			BucketSettings {512ul, 8ul * 1024},
			BucketSettings {1024ul, 8ul * 1024},
			BucketSettings {2048ul, 4ul * 1024},
			BucketSettings {4096ul, 4ul * 1024},
			BucketSettings {8192ul, 256ul},
			BucketSettings {16384ul, 128ul},
			BucketSettings {16384ul * 2ul, 64ul},
			BucketSettings {16384ul * 2ul, 64ul},
		};

		static constexpr int BucketsCount = static_cast<int>(s_BucketSettings.size());

		static inline int64_t s_ObjectsCount {0};
		static inline bool s_WasInit {false};

		static int GetBucketIndex(const size_t size)
		{
			for (int i = 0; i < BucketsCount; i++)
			{
				if (size <= s_BucketSettings[i].elementSize)
				{
					return i;
				}
			}

			return -1;
		}


		template<int bucketIndex>
		class Bucket
		{
			constexpr static size_t BucketElementSize = s_BucketSettings[bucketIndex].elementSize;
			constexpr static size_t BucketElementsCount = s_BucketSettings[bucketIndex].elementsCount;
			constexpr static size_t BucketSize = BucketElementSize * BucketElementsCount;

			using PageElement = std::byte[BucketElementSize];

			struct Page
			{
				PageElement data[BucketElementsCount];
			};

		public:

			static void Init()
			{
				static Bucket instance;
			}


			static Bucket* GetInstance()
			{
				return s_pInstance;
			}

			Bucket()
			{
				// Pre-reserve for one page worth of elements
				std::vector<void*> storage;
				storage.reserve(BucketElementsCount);
				m_freeElements = std::stack(std::move(storage));
				s_pInstance = this;
			}

			~Bucket()
			{
				s_pInstance = nullptr;
			}


			static bool WasShutDown() {return s_pInstance == nullptr;}


			void* Allocate()
			{
				if (m_freeElements.empty())
				{
					AddPage();
				}

				auto* pResult = m_freeElements.top();
				m_freeElements.pop();
				return pResult;
			}


			void Deallocate(void* pElement)
			{
				m_freeElements.push(pElement);
			}


			void AddPage()
			{
				VSBLOG_INFO("Adding pool page: {} (element size: {})", m_pagesUsed, BucketElementSize);

				// Heap-allocate the page with proper alignment
				auto* pPage = static_cast<Page*>(::operator new(sizeof(Page), std::align_val_t{alignof(std::max_align_t)}));

				// Store for potential cleanup (optional, since we're not freeing)
				m_allocatedPages.push_back(pPage);

				for (auto& element : pPage->data)
				{
					m_freeElements.push(&element);
				}

				m_pagesUsed++;
			}

		private:

			static constinit inline Bucket* s_pInstance {nullptr};
			int m_pagesUsed {0};
			std::stack<void*, std::vector<void*>> m_freeElements {};
			std::vector<Page*> m_allocatedPages {};
		};
	};
}
