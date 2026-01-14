//
// Created by Alexander on 14.01.2026.
//

#pragma once
#include <cstdlib>
#include <array>
#include <stack>
#include <vector>
#include "vsb/log.h"

namespace flying_circles
{
	template<int MaxN, typename F>
	constexpr void TemplateIndexedDispatch(int n, F&& func)
	{
		[&]<std::size_t... Is>(std::index_sequence<Is...>)
		{
			(void)((Is == n ? (func.template operator()<Is>(), true) : false) || ...);
		}(std::make_index_sequence<MaxN>{});
	}


	class BasicMemoryPool
	{
	public:

		static int64_t GetObjectsCount() {return s_ObjectsCount;}

		static void* Allocate(const size_t size)
		{
			auto bucketIndex = GetBucketIndex(size);

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
			auto bucketIndex = GetBucketIndex(size);

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

		static constexpr std::array s_BucketElementSizes {8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 16384 * 2, 16384 * 2};
		static constexpr std::array s_BucketElementsCounts {64 * 1024, 64 * 1024, 64 * 1024, 32 * 1024, 16 * 1024, 8 * 1024, 8 * 1024, 8 * 1024, 4 * 1024, 4 * 1024, 256, 128, 64, 64};

		static_assert(s_BucketElementSizes.size() == s_BucketElementsCounts.size(), "sizes should match!");

		static inline int64_t s_ObjectsCount {};

		static int GetBucketIndex(const size_t size)
		{
			for (int i = 0; i < s_BucketElementSizes.size(); i++)
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
			constexpr static int BucketElementSize = s_BucketElementSizes[bucketIndex];
			constexpr static int BucketElementsCount = s_BucketElementsCounts[bucketIndex];
			constexpr static int BucketSize = BucketElementSize * BucketElementsCount;

			using PageElement = std::byte[BucketElementSize];

			using Page = PageElement[BucketElementsCount];

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

				for (auto& element : *pPage)
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
