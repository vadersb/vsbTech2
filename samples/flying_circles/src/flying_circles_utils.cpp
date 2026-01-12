//
// Created by Alexander on 12.01.2026.
//

#include "flying_circles_utils.h"

#include <climits>
#include <cstdlib>

#include "raylib.h"


float fc_utils::GetRandomFloat(float min, float max)
{
	const float scale = GetRandomFloat01();
	return min + scale * (max - min);
}


float fc_utils::GetRandomFloat01()
{
	return static_cast<float>(GetRandomValue(0, INT_MAX)) / static_cast<float>(INT_MAX);
}
