//
// Created by Alexander on 12.01.2026.
//

#pragma once


namespace params
{
	constexpr int WindowWidth = 1280;
	constexpr int WindowHeight = 800;

	constexpr int ObjectsPerFrame = 2;

	constexpr float RegularLifetimeMin = 20.0f;
	constexpr float RegularLifetimeMax = 80.0f;

	constexpr float LongLifetimeMin = 360.0f;
	constexpr float LongLifetimeMax = 60.0f * 12.0f;
	constexpr float LongLifeChance = 0.25f;

	constexpr int MinArraySize = 100;
	constexpr int MaxArraySize = 1000;

	constexpr float PeriodBetweenGens = 2.0f;

	constexpr int ManualObjectPackGenCount = 1000;

}