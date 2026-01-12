//
// Created by Alexander on 12.01.2026.
//


#include "vsb/log.h"
#include "vsb/debug.h"
#include "raylib.h"

void update();
void render();


int main()
{
	vsb::log::Init(true);

	SetConfigFlags(FLAG_VSYNC_HINT);
	InitWindow(1280, 800, "Flying Circles");

	while (!WindowShouldClose())
	{
		update();
		render();
	}

	CloseWindow();

	vsb::log::Uninit();
}


void update()
{

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