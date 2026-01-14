//
// Created by Alexander on 12.01.2026.
//


#include "vsb/log.h"
#include "flying_circles_settings.h"
#include "raylib.h"
#include "flying_circle.h"
#include "flying_circles_utils.h"
#include "basic_allocator.h"

using namespace params;
using namespace vsb;
using namespace fc_utils;

namespace
{
	void update();
	void render();

	float s_TimeSinceStart = 0.0f;

	uint64_t s_TotalObjectsCreated = 0;

	bool s_SlowGenRate = false;
	bool s_AutoGenMode = false;
	float s_TimeSinceLastGen = 0.0f;

	std::vector<flying_circles::PooledVector<int>> s_GarbageArrays {};
	std::vector<FlyingCircle*> s_Circles {};

	std::vector<FlyingCircle*> s_CurFrameCircles {};
	uint64_t s_CurFrameMaxCircleCount = 0;
	uint64_t s_CurFrameMaxValueCount = 0;

	uint64_t s_CurFrameCurCircleCount = 0;
	uint64_t s_CurFrameCurValueCount = 0;
	

	uint64_t s_LastUpdateMicros = 0;
	constexpr int LastUpdateTimesCount = 120;
	std::array<uint64_t, LastUpdateTimesCount> s_UpdateTimesHistory {};
	int s_UpdateTimesIndex = 0;


	void add_circle()
	{
		const float lifetime = GetRandomFloat01() < LongLifeChance
			? GetRandomFloat(LongLifetimeMin, LongLifetimeMax)
			: GetRandomFloat(RegularLifetimeMin, RegularLifetimeMax);

		auto* pCircle = new FlyingCircle(lifetime, MinArraySize, MaxArraySize, s_Circles);
		s_Circles.push_back(pCircle);
		s_TotalObjectsCreated++;
	}


	void process_circles_gen(const float timeDelta)
	{
		// Determine if we should spawn this frame
		bool shouldSpawn = true;
		if (s_SlowGenRate)
		{
			s_TimeSinceLastGen += timeDelta;
			if (s_TimeSinceLastGen >= PeriodBetweenGens)
			{
				s_TimeSinceLastGen = 0.0f;
				shouldSpawn = true;
			}
			else
			{
				shouldSpawn = false;
			}
		}

		if (shouldSpawn)
		{
			for (int i = 0; i < ObjectsPerFrame; i++)
			{
				add_circle();
			}
		}
	}


	void circles_update(const float timeDelta)
	{
		const int count = static_cast<int>(s_Circles.size());

		bool hasRemovals = false;

		for (int i = count - 1; i >= 0; i--)
		{
			auto* pCircle = s_Circles[i];
			pCircle->Update(timeDelta);

			if (pCircle->IsDead())
			{
				pCircle->Destroy();
				s_Circles[i] = nullptr;
				hasRemovals = true;
				//s_Circles.erase(s_Circles.begin() + i);
			}
		}

		if (hasRemovals)
		{
			std::erase(s_Circles, nullptr);
		}

	}
	
	
	void per_frame_circles_processing()
	{
		//Per frame circles processing
		for (auto* circle: s_Circles)
		{
			if (circle->GetOtherCirclesCount() % 2 == 1) continue;

			s_CurFrameCircles.push_back(circle);
		}

		std::stable_sort(s_CurFrameCircles.begin(), s_CurFrameCircles.end(),
		                 [](const FlyingCircle* a, const FlyingCircle* b)
		                 {
			                 // Add your comparison logic here based on how FlyingCircle should be sorted
			                 return a->GetOtherCirclesCount() < b->GetOtherCirclesCount(); // Example comparison
		                 });

		s_CurFrameCurCircleCount = s_CurFrameCircles.size();

		s_CurFrameMaxCircleCount = std::max(s_CurFrameMaxCircleCount, s_CurFrameCurCircleCount);

		s_CurFrameCurValueCount = 0;

		for (auto* circle: s_CurFrameCircles)
		{
			s_CurFrameCurValueCount += circle->GetOtherCirclesCount();
		}

		s_CurFrameMaxValueCount = std::max(s_CurFrameMaxValueCount, s_CurFrameCurValueCount);

		s_CurFrameCircles.clear();
	}
}


int main()
{
	log::Init(true);

	SetConfigFlags(FLAG_VSYNC_HINT);
	InitWindow(WindowWidth, WindowHeight, "Flying Circles");

	s_Circles.reserve(32 * 1024);
	s_CurFrameCircles.reserve(s_Circles.capacity());
	s_GarbageArrays.reserve(512);

	while (!WindowShouldClose())
	{
		auto updateStartTime = std::chrono::high_resolution_clock::now();
		update();
		auto updateEndTime = std::chrono::high_resolution_clock::now();
		s_LastUpdateMicros = std::chrono::duration_cast<std::chrono::microseconds>(updateEndTime - updateStartTime).count();

		// Store in circular buffer
		s_UpdateTimesHistory[s_UpdateTimesIndex] = s_LastUpdateMicros;
		s_UpdateTimesIndex = (s_UpdateTimesIndex + 1) % LastUpdateTimesCount;

		render();

		DestructionCentral::ProcessDefault();
	}

	CloseWindow();

	log::Uninit();
}


namespace
{
	void update()
	{
		float const dt = GetFrameTime();
		s_TimeSinceStart += dt;




		// Spawn new circles
		process_circles_gen(dt);


		//doing some garbage
		const int garbageArraySize = GetRandomValue(5, 100);

		flying_circles::PooledVector<int> garbageArray(garbageArraySize, 0);
		s_GarbageArrays.push_back(std::move(garbageArray));

		if (s_GarbageArrays.size() > 100)
		{
			if (GetRandomValue(0, 10) < 1)
			{
				s_GarbageArrays.clear();
			}
		}


		// Update and remove dead circles
		circles_update(dt);

		//Per frame circles processing
		per_frame_circles_processing();
	}


	void render()
	{
		BeginDrawing();
		ClearBackground(BLACK);

		for (const auto* pCircle : s_Circles)
		{
			pCircle->Draw();
		}

		//---
		//UI OVERLAY
		DrawRectangle(5, 5, 500, 620, {25, 25, 25, 220});

		DrawText(TextFormat("Total Objects Created: %llu", s_TotalObjectsCreated), 10, 10, 20, WHITE);
		DrawText(TextFormat("Objects: %zu", s_Circles.size()), 10, 40, 30, WHITE);


		// Update time bars: 1 pixel = 10 microseconds
		// Draw all historical bars with low alpha (ghosted)
		for (int i = 0; i < LastUpdateTimesCount; i++)
		{
			constexpr Color ghostColor = {135, 206, 235, 25};
			const int barWidth = static_cast<int>(s_UpdateTimesHistory[i] / 10);
			DrawRectangle(10, 70, barWidth, 16, ghostColor);
		}
		// Draw current bar on top with full color
		const int updateBarWidth = static_cast<int>(s_LastUpdateMicros / 10);
		DrawRectangle(10, 70, updateBarWidth, 16, SKYBLUE);


		//DrawText(TextFormat("Uptime: %s", FormatTimeHMS(s_TimeSinceStart)), 10, 470, 20, BEIGE);

		Color fadedColor = BEIGE;
		fadedColor.a = 55;

		DrawText(TextFormat("Max Circle Count: %llu", s_CurFrameMaxCircleCount), 10, 500, 20, BEIGE);
		DrawText(TextFormat("%llu", s_CurFrameCurCircleCount), 350, 500, 20, fadedColor);
		DrawText(TextFormat("Max Value Count: %llu", s_CurFrameMaxValueCount), 10, 530, 20, BEIGE);
		DrawText(TextFormat("%llu", s_CurFrameCurValueCount), 350, 530, 20, fadedColor);
		
		
		DrawFPS(10, 570);

		DrawText(TextFormat("Pooled Objects: %llu", flying_circles::BasicMemoryPool::GetObjectsCount()), 10, 600, 20, BEIGE);

		EndDrawing();
	}
}

