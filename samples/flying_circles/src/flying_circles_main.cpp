//
// Created by Alexander on 12.01.2026.
//


#include "vsb/log.h"
#include "vsb/objects/ptr.h"
#include "flying_circles_settings.h"
#include "raylib.h"
#include "flying_circle.h"
#include "flying_circles_utils.h"
#include "flying_circles_shaders.h"
#include "rlgl.h"
#include <algorithm>

using namespace params;
using namespace vsb;
using namespace fc_utils;

namespace
{
	using SortableCircle = PointerWithIndex<FlyingCircle>;


	void update();
	void render();

	float s_TimeSinceStart = 0.0f;

	i64 s_TotalObjectsCreated = 0;

	bool s_SlowGenRate = false;
	bool s_AutoGenMode = false;
	float s_TimeSinceLastGen = 0.0f;

	std::vector<vsb::vector<int>> s_GarbageArrays {};
	std::vector<FlyingCirclePtr> s_Circles {};

	std::vector<SortableCircle> s_CurFrameCircles {};
	u64 s_CurFrameMaxCircleCount = 0;
	u64 s_CurFrameMaxValueCount = 0;

	u64 s_CurFrameCurCircleCount = 0;
	u64 s_CurFrameCurValueCount = 0;
	

	u64 s_LastUpdateMicros = 0;
	constexpr int LastUpdateTimesCount = 180;
	std::array<u64, LastUpdateTimesCount> s_UpdateTimesHistory {};
	int s_UpdateTimesIndex = 0;

	u64 s_LastRenderMicros = 0;
	constexpr int LastRenderTimesCount = LastUpdateTimesCount;
	std::array<u64, LastRenderTimesCount> s_RenderTimesHistory {};
	int s_RenderTimesIndex = 0;

	i64 s_AutoGenBaseCount = 1000;
	i64 s_AutoGenRange = 1500;

	Shader s_CurShader;

	Shader s_CircleShader;
	Shader s_AltCircleShader;
	bool s_UseAltShader = false;


	void add_circle()
	{
		const float lifetime = GetRandomFloat01() < LongLifeChance
			? GetRandomFloat(LongLifetimeMin, LongLifetimeMax)
			: GetRandomFloat(RegularLifetimeMin, RegularLifetimeMax);

		auto* pCircle = new FlyingCircle(lifetime, MinArraySize, MaxArraySize, s_Circles);
		s_Circles.emplace_back(pCircle);
		s_TotalObjectsCreated++;
	}


	void process_inputs()
	{
		if (IsKeyPressed(KEY_A))
		{
			s_AutoGenMode = !s_AutoGenMode;
		}

		// Adjust auto gen base count with Left/Right arrows
		bool isShiftPressed = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);

		if (IsKeyPressed(KEY_LEFT))
		{
			const i64 delta = isShiftPressed ? 1000 : 100;
			s_AutoGenBaseCount -= delta;
			s_AutoGenBaseCount = std::max<i64>(0, s_AutoGenBaseCount);
		}
		if (IsKeyPressed(KEY_RIGHT))
		{
			const i64 delta = isShiftPressed ? 1000 : 100;
			s_AutoGenBaseCount += delta;
		}

		// Adjust auto gen range with Up/Down arrows


		if (IsKeyPressed(KEY_UP))
		{
			i64 delta = isShiftPressed ? 1000 : 100;

			s_AutoGenRange += delta;
		}
		if (IsKeyPressed(KEY_DOWN))
		{
			i64 delta = isShiftPressed ? 1000 : 100;

			s_AutoGenRange -= delta;

			s_AutoGenRange = std::max<i64>(0, s_AutoGenRange);
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
			const int toAdd = isShiftPressed ? ManualObjectPackGenCount * 10 : ManualObjectPackGenCount;

			for (int i = 0; i < toAdd; i++)
			{
				add_circle();
			}
		}

		if (IsKeyPressed(KEY_S))
		{
			s_UseAltShader = !s_UseAltShader;
			s_CurShader = s_UseAltShader ? s_AltCircleShader : s_CircleShader;
		}
	}


	void process_auto_gen()
	{
		u64 autoGenMin = s_AutoGenBaseCount;
		u64 autoGenMax = s_AutoGenBaseCount + s_AutoGenRange;

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
			auto* pCircle = s_Circles[i].Get();
			pCircle->Update(timeDelta);

			if (pCircle->IsDead())
			{
				pCircle->Destroy();
				s_Circles[i].Reset();
				hasRemovals = true;
				//s_Circles.erase(s_Circles.begin() + i);
			}
		}

		if (hasRemovals)
		{
			std::erase(s_Circles, Ptr<FlyingCircle>{});
		}

		for (auto pCircle: s_Circles)
		{
			pCircle->PostUpdate();
		}
	}


	void remove_all_circles()
	{
		for (auto pCircle : s_Circles)
		{
			pCircle->Destroy();
		}

		s_Circles.clear();
	}
	
	
	void per_frame_circles_processing()
	{
		//Per frame circles processing
		for (int i = 0; i < static_cast<int>(s_Circles.size()); i++)
		{
			auto* circle = s_Circles[i].Get();

			if (circle->GetOtherCirclesCount() % 2 == 1) continue;

			s_CurFrameCircles.push_back({circle, i});
		}


		std::ranges::sort(s_CurFrameCircles,
		                  [](const SortableCircle& a, const SortableCircle& b)
		                  {
			                  const auto countA = a.pPointer->GetOtherCirclesCount();
			                  const auto countB = b.pPointer->GetOtherCirclesCount();

			                  if (countA != countB)
			                  {
				                  return countA < countB;
			                  }

			                  return a.index < b.index;
		                  });


		s_CurFrameCurCircleCount = s_CurFrameCircles.size();

		s_CurFrameMaxCircleCount = std::max(s_CurFrameMaxCircleCount, s_CurFrameCurCircleCount);

		s_CurFrameCurValueCount = 0;

		for (auto& entry: s_CurFrameCircles)
		{
			s_CurFrameCurValueCount += entry.pPointer->GetOtherCirclesCount();
		}

		s_CurFrameMaxValueCount = std::max(s_CurFrameMaxValueCount, s_CurFrameCurValueCount);

		s_CurFrameCircles.clear();
	}
}


int main()
{
	log::Init(true, "flying_circles.log");
	VSBLOG_INFO("");
	VSBLOG_INFO("");
	VSBLOG_INFO("");
	VSBLOG_INFO("                   ----------=== FLYING CIRCLES SAMPLE STARTED ===----------                    ");
	VSBInit();

	std::string windowTitle = "Flying Circles [";
	windowTitle.append(__TIMESTAMP__);
	windowTitle.append("]");

	SetTraceLogCallback(MyRaylibLogCallback);
	SetConfigFlags(FLAG_VSYNC_HINT);
	InitWindow(WindowWidth, WindowHeight, windowTitle.c_str());
	SetTargetFPS(0);

	s_CircleShader = LoadShaderFromMemory(flying_circles::CircleVertexShader, flying_circles::CircleFragmentShader);
	s_AltCircleShader = LoadShaderFromMemory(flying_circles::CircleVertexShader, flying_circles::AltCircleFragmentShader);

	s_CurShader = s_CircleShader;

	s_Circles.reserve(32 * 1024);
	s_CurFrameCircles.reserve(s_Circles.capacity());
	s_GarbageArrays.reserve(512);

	while (!WindowShouldClose())
	{
		auto updateStartTime = std::chrono::high_resolution_clock::now();
		update();
		auto updateEndTime = std::chrono::high_resolution_clock::now();
		s_LastUpdateMicros = std::chrono::duration_cast<std::chrono::microseconds>(updateEndTime - updateStartTime).count();

		// Store in a circular buffer
		s_UpdateTimesHistory[s_UpdateTimesIndex] = s_LastUpdateMicros;
		s_UpdateTimesIndex = (s_UpdateTimesIndex + 1) % LastUpdateTimesCount;

		auto renderStartTime = std::chrono::high_resolution_clock::now();
		render();
		auto renderEndTime = std::chrono::high_resolution_clock::now();

		EndDrawing();

		s_LastRenderMicros = std::chrono::duration_cast<std::chrono::microseconds>(renderEndTime - renderStartTime).count();

		s_RenderTimesHistory[s_RenderTimesIndex] = s_LastRenderMicros;
		s_RenderTimesIndex = (s_RenderTimesIndex + 1) % LastRenderTimesCount;

		DestructionCentral::ProcessDefault();
	}

	remove_all_circles();

	UnloadShader(s_CircleShader);
	UnloadShader(s_AltCircleShader);
	CloseWindow();

	VSBUninit();
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

		vsb::vector<int> garbageArray(garbageArraySize, 0);
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


	const char* FormatTimeHMS(float timeSinceStart)
	{
		int totalSeconds = static_cast<int>(timeSinceStart);
		int hours = totalSeconds / 3600;
		int minutes = (totalSeconds % 3600) / 60;
		int seconds = totalSeconds % 60;
		return TextFormat("%d:%02d:%02d", hours, minutes, seconds);
	}



	void render()
	{
		BeginDrawing();
		ClearBackground(BLACK);

		BeginShaderMode(s_CurShader);

		rlBegin(RL_QUADS);

		for (const auto& pCircle : s_Circles)
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



		// Update time bars: 1 pixel = 10 microseconds
		// Draw all historical bars with low alpha (ghosted)
		u64 maxUpdateTime = 0;
		for (int i = 0; i < LastUpdateTimesCount; i++)
		{
			if (s_UpdateTimesHistory[i] > maxUpdateTime)
				maxUpdateTime = s_UpdateTimesHistory[i];
		}

		u64 maxRenderTime = 0;
		for (int i = 0; i < LastRenderTimesCount; i++)
		{
			if (s_RenderTimesHistory[i] > maxRenderTime)
				maxRenderTime = s_RenderTimesHistory[i];
		}

		int maxBarWidth = static_cast<int>(maxUpdateTime / 10);
		constexpr Color maxColor(255, 0, 0, 80);
		DrawRectangle(10, 70, maxBarWidth, 16, maxColor);

		for (int i = 0; i < LastUpdateTimesCount; i++)
		{
			constexpr Color ghostColor = {135, 206, 235, 25};
			const int barWidth = static_cast<int>(s_UpdateTimesHistory[i] / 10);
			DrawRectangle(10, 70, barWidth, 16, ghostColor);
		}
		// Draw the current bar on top with full color
		const int updateBarWidth = static_cast<int>(s_LastUpdateMicros / 10);
		DrawRectangle(10, 70, updateBarWidth, 16, SKYBLUE);

		//update and render times
		DrawText(TextFormat("Update time: %i", maxUpdateTime), 10, 120, 20, WHITE);
		DrawText(TextFormat("Render time: %i", maxRenderTime), 10, 150, 20, WHITE);

		//target fps
		const i64 fullFrameTime = static_cast<i64>(1 + maxUpdateTime + maxRenderTime);

		const i64 targetFps = 1'000'000 / fullFrameTime;

		constexpr Color TargetFPSColor(10, 150, 10, 180);

		DrawText(TextFormat("Target FPS: %i", targetFps), 320, 40, 20, TargetFPSColor);


		//---
		DrawText(TextFormat("[X] Slow Gen Rate: %s", s_SlowGenRate ? "ON" : "OFF"), 10, 350, 20,
		         s_SlowGenRate ? ORANGE : GREEN);
		DrawText(TextFormat("[Q] Generate %d objects", ManualObjectPackGenCount), 10, 380, 20, GREEN);

		int autoGenMin = static_cast<int>(s_AutoGenBaseCount);
		int autoGenMax = static_cast<int>(s_AutoGenBaseCount + s_AutoGenRange);
		DrawText(TextFormat("[A] Auto Gen (%d - %d): %s", autoGenMin, autoGenMax, s_AutoGenMode ? "ON" : "OFF"), 10,
		         410, 20,
		         s_AutoGenMode ? ORANGE : GREEN);
		DrawText("[LR] Base Count  [UD] Range", 10, 440, 20, GRAY);

		DrawText(TextFormat("Uptime: %s", FormatTimeHMS(s_TimeSinceStart)), 10, 470, 20, BEIGE);

		Color fadedColor = BEIGE;
		fadedColor.a = 55;

		DrawText(TextFormat("Max Circle Count: %llu", s_CurFrameMaxCircleCount), 10, 500, 20, BEIGE);
		DrawText(TextFormat("%llu", s_CurFrameCurCircleCount), 350, 500, 20, fadedColor);
		DrawText(TextFormat("Max Value Count: %llu", s_CurFrameMaxValueCount), 10, 530, 20, BEIGE);
		DrawText(TextFormat("%llu", s_CurFrameCurValueCount), 350, 530, 20, fadedColor);
		
		
		DrawFPS(10, 570);

		DrawText(TextFormat("Pooled Objects: %llu", vsb::memory::SingleThreadedPool::GetObjectsCount()), 10, 600, 20, BEIGE);


	}
}
