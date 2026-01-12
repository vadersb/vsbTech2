//
// Created by Alexander on 12.01.2026.
//


#include "vsb/log.h"
#include "flying_circles_settings.h"
#include "raylib.h"

namespace
{
	void update();
	void render();

	float s_TimeSinceStart = 0.0f;

	std::vector<std::vector<int>> s_GarbageArrays;

}

using namespace params;
using namespace vsb;


int main()
{
	log::Init(true);

	SetConfigFlags(FLAG_VSYNC_HINT);
	InitWindow(WindowWidth, WindowHeight, "Flying Circles");

	while (!WindowShouldClose())
	{
		update();
		render();
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



		//doing some garbage
		const int garbageArraySize = GetRandomValue(5, 100);

		std::vector garbageArray(garbageArraySize, 0);
		s_GarbageArrays.push_back(std::move(garbageArray));

		if (s_GarbageArrays.size() > 100)
		{
			if (GetRandomValue(0, 10) < 1)
			{
				s_GarbageArrays.clear();
			}
		}

	}


	void render()
	{
		BeginDrawing();
		ClearBackground(BLACK);


		//---
		//UI OVERLAY
		DrawRectangle(5, 5, 500, 620, {25, 25, 25, 220});


		DrawFPS(10, 570);


		EndDrawing();
	}
}

