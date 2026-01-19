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

		static std::int64_t GetObjectsCount() {return s_ObjectsCount;}

		static void* Allocate(const size_t size)
		{
			VSB_CHECK_THREAD();

			const auto bucketIndex = GetBucketIndex(size);

			if (bucketIndex == -1)
			{
				return std::malloc(size);
			}

			void* pResult;

			TemplateIndexedDispatch<s_BucketElementSizes.size()>(bucketIndex, [&pResult]<std::size_t index>()
			{
				auto& bucket = GetBucket<index>();
				pResult = bucket.Allocate();
			});

			s_ObjectsCount++;
			return pResult;
		}


		static void Deallocate(void* pElement, const size_t size)
		{
			VSB_CHECK_THREAD();

			const auto bucketIndex = GetBucketIndex(size);

			if (bucketIndex == -1)
			{
				std::free(pElement);
				return;
			}

			TemplateIndexedDispatch<s_BucketElementSizes.size()>(bucketIndex, [pElement]<std::size_t index>()
			{
				if (!Bucket<index>::WasShutDown())
				{
					GetBucket<index>().Deallocate(pElement);
				}
			});

			s_ObjectsCount--;
		}

	private:

		static constexpr std::array s_BucketElementSizes {8ul, 16ul, 32ul, 64ul, 128ul, 256ul, 512ul, 1024ul, 2048ul, 4096ul, 8192ul, 16384ul, 16384ul * 2ul, 16384ul * 2ul};
		static constexpr std::array s_BucketElementsCounts {64ul * 1024, 64ul * 1024, 64ul * 1024, 32ul * 1024, 16ul * 1024, 8ul * 1024, 8ul * 1024, 8ul * 1024, 4ul * 1024, 4ul * 1024, 256ul, 128ul, 64ul, 64ul};

		static_assert(s_BucketElementSizes.size() == s_BucketElementsCounts.size(), "sizes should match!");

		static inline int64_t s_ObjectsCount {0};

		static int GetBucketIndex(const size_t size)
		{
			constexpr int BucketsCount = static_cast<const int>(s_BucketElementSizes.size());

			for (int i = 0; i < BucketsCount; i++)
			{
				if (size <= s_BucketElementSizes[i])
				{
					return i;
				}
			}

			return -1;
		}


		template<int bucketIndex>
		class Bucket
		{
			constexpr static size_t BucketElementSize = s_BucketElementSizes[bucketIndex];
			constexpr static size_t BucketElementsCount = s_BucketElementsCounts[bucketIndex];
			constexpr static size_t BucketSize = BucketElementSize * BucketElementsCount;

			using PageElement = std::byte[BucketElementSize];

			struct Page
			{
				PageElement data[BucketElementsCount];
			};

		public:

			Bucket()
			{
				// Pre-reserve for one page worth of elements
				std::vector<void*> storage;
				storage.reserve(BucketElementsCount);
				m_freeElements = std::stack(std::move(storage));
			}

			~Bucket()
			{
				s_WasShutDown = true;
			}


			static bool WasShutDown() {return s_WasShutDown;}


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
				GetAllocatedPages().push_back(pPage);

				for (auto& element : pPage->data)
				{
					m_freeElements.push(&element);
				}

				m_pagesUsed++;
			}

		private:

			// SIOF-safe storage for allocated pages
			static std::vector<Page*>& GetAllocatedPages()
			{
				static std::vector<Page*> pages;
				return pages;
			}

			inline static bool s_WasShutDown {false};
			int m_pagesUsed {0};
			std::stack<void*, std::vector<void*>> m_freeElements {};
		};


		template<int bucketIndex>
		static Bucket<bucketIndex>& GetBucket()
		{
			static Bucket<bucketIndex> bucket;
			return bucket;
		}

	};
}
