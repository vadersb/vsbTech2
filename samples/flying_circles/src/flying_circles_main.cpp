//
// Created by Alexander on 12.01.2026.
//


#include "vsb/log.h"
#include "flying_circles_settings.h"
#include "raylib.h"
#include "flying_circle.h"
#include "flying_circles_utils.h"
#include "basic_allocator.h"
#include "flying_circles_shaders.h"
#include "rlgl.h"

using namespace params;
using namespace vsb;
using namespace fc_utils;

namespace
{
	struct SortableCircle
	{
		FlyingCircle* pCircle;
		int originalIndex;
	};

	void update();
	void render();

	float s_TimeSinceStart = 0.0f;

	uint64_t s_TotalObjectsCreated = 0;

	bool s_SlowGenRate = false;
	bool s_AutoGenMode = false;
	float s_TimeSinceLastGen = 0.0f;

	std::vector<flying_circles::PooledVector<int>> s_GarbageArrays {};
	std::vector<FlyingCircle*> s_Circles {};

	std::vector<SortableCircle> s_CurFrameCircles {};
	uint64_t s_CurFrameMaxCircleCount = 0;
	uint64_t s_CurFrameMaxValueCount = 0;

	uint64_t s_CurFrameCurCircleCount = 0;
	uint64_t s_CurFrameCurValueCount = 0;
	

	uint64_t s_LastUpdateMicros = 0;
	constexpr int LastUpdateTimesCount = 120;
	std::array<uint64_t, LastUpdateTimesCount> s_UpdateTimesHistory {};
	int s_UpdateTimesIndex = 0;

	uint64_t s_AutoGenBaseCount = 1000;
	uint64_t s_AutoGenRange = 1500;


	Shader s_CircleShader;


	void add_circle()
	{
		const float lifetime = GetRandomFloat01() < LongLifeChance
			? GetRandomFloat(LongLifetimeMin, LongLifetimeMax)
			: GetRandomFloat(RegularLifetimeMin, RegularLifetimeMax);

		auto* pCircle = new FlyingCircle(lifetime, MinArraySize, MaxArraySize, s_Circles);
		s_Circles.push_back(pCircle);
		s_TotalObjectsCreated++;
	}


	void process_inputs()
	{
		if (IsKeyPressed(KEY_A))
		{
			s_AutoGenMode = !s_AutoGenMode;
		}

		// Adjust auto gen base count with Left/Right arrows
		if (IsKeyPressed(KEY_LEFT))
		{
			s_AutoGenBaseCount = std::max(static_cast<uint64_t>(0), s_AutoGenBaseCount - 100);
		}
		if (IsKeyPressed(KEY_RIGHT))
		{
			s_AutoGenBaseCount += 100;
		}

		// Adjust auto gen range with Up/Down arrows
		if (IsKeyPressed(KEY_UP))
		{
			s_AutoGenRange += 100;
		}
		if (IsKeyPressed(KEY_DOWN))
		{
			s_AutoGenRange = std::max(static_cast<uint64_t>(100), s_AutoGenRange - 100);
		}

		// Toggle slow generation rate
		if (IsKeyPressed(KEY_X))
		{
			s_SlowGenRate = !s_SlowGenRate;
			s_TimeSinceLastGen = 0.0f;
		}

		// Generate a pack of objects instantly
		if (IsKeyPressed(KEY_Q))
		{
			for (int i = 0; i < ManualObjectPackGenCount; i++)
			{
				add_circle();
			}
		}
	}


	void process_auto_gen()
	{
		size_t autoGenMin = s_AutoGenBaseCount;
		size_t autoGenMax = s_AutoGenBaseCount + s_AutoGenRange;

		if (s_Circles.size() <= autoGenMin && s_SlowGenRate)
		{
			s_SlowGenRate = false;
		}

		if (s_Circles.size() >= autoGenMax)
		{
			s_SlowGenRate = true;
		}
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
		for (auto i = 0; i < s_Circles.size(); i++)
		{
			auto* circle = s_Circles[i];

			if (circle->GetOtherCirclesCount() % 2 == 1) continue;

			s_CurFrameCircles.push_back({circle, i});
		}

		std::sort(s_CurFrameCircles.begin(), s_CurFrameCircles.end(),
		                 [](const SortableCircle& a, const SortableCircle& b)
		                 {
		                 	const auto countA = a.pCircle->GetOtherCirclesCount();
		                 	const auto countB = b.pCircle->GetOtherCirclesCount();

		                 	if (countA != countB)
		                 	{
		                 		return countA < countB;
		                 	}

		                 	return a.originalIndex < b.originalIndex;
		                 });

		s_CurFrameCurCircleCount = s_CurFrameCircles.size();

		s_CurFrameMaxCircleCount = std::max(s_CurFrameMaxCircleCount, s_CurFrameCurCircleCount);

		s_CurFrameCurValueCount = 0;

		for (auto& entry: s_CurFrameCircles)
		{
			s_CurFrameCurValueCount += entry.pCircle->GetOtherCirclesCount();
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
	SetTargetFPS(0);

	s_CircleShader = LoadShaderFromMemory(flying_circles::CircleVertexShader, flying_circles::CircleFragmentShader);

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

	UnloadShader(s_CircleShader);
	CloseWindow();

	log::Uninit();
}


namespace
{
	void update()
	{
		float const dt = GetFrameTime();
		s_TimeSinceStart += dt;

		process_inputs();

		if (s_AutoGenMode)
		{
			process_auto_gen();
		}

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

		BeginShaderMode(s_CircleShader);

		rlBegin(RL_QUADS);

		for (const auto* pCircle : s_Circles)
		{
			pCircle->DrawQuad();
		}

		rlEnd();
		EndShaderMode();

		//---
		//UI OVERLAY
		DrawRectangle(5, 5, 500, 620, {25, 25, 25, 220});

		DrawText(TextFormat("Total Objects Created: %llu", s_TotalObjectsCreated), 10, 10, 20, WHITE);
		DrawText(TextFormat("Objects: %zu", s_Circles.size()), 10, 40, 30, WHITE);

		DrawText(TextFormat("Update time (ms): %i", s_UpdateTimesHistory[s_UpdateTimesIndex] / 1000), 10, 120, 20, WHITE);

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

		DrawText(TextFormat("[X] Slow Gen Rate: %s", s_SlowGenRate ? "ON" : "OFF"), 10, 350, 20,
		         s_SlowGenRate ? ORANGE : GREEN);
		DrawText(TextFormat("[Q] Generate %d objects", ManualObjectPackGenCount), 10, 380, 20, GREEN);

		int autoGenMin = static_cast<int>(s_AutoGenBaseCount);
		int autoGenMax = static_cast<int>(s_AutoGenBaseCount + s_AutoGenRange);
		DrawText(TextFormat("[A] Auto Gen (%d - %d): %s", autoGenMin, autoGenMax, s_AutoGenMode ? "ON" : "OFF"), 10,
		         410, 20,
		         s_AutoGenMode ? ORANGE : GREEN);
		DrawText("[LR] Base Count  [UD] Range", 10, 440, 20, GRAY);

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

