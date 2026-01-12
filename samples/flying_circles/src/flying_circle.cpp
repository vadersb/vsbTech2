//
// Created by Alexander on 12.01.2026.
//

#include "flying_circle.h"
#include "flying_circles_settings.h"
#include "flying_circles_utils.h"
#include "raylib.h"
#include "vsb/objects/safe_ptr.h"


FlyingCircle::FlyingCircle(float lifetime, int minArraySize, int maxArraySize, const std::vector<FlyingCircle*>& allCircles) :
	m_x(fc_utils::GetRandomFloat(0.0f, params::WindowWidth)),
	m_y(fc_utils::GetRandomFloat(0.0f, params::WindowHeight)),
	m_velX((fc_utils::GetRandomFloat01() - 0.5f) * 100.0f),
	m_velY((fc_utils::GetRandomFloat01() - 0.5f) * 100.0f),
	m_radius(5.0f + fc_utils::GetRandomFloat01() * 15.0f),
	m_lifetime(lifetime)
{
	m_baseColor.r = GetRandomValue(100, 256);
	m_baseColor.g = GetRandomValue(100, 256);
	m_baseColor.b = GetRandomValue(100, 256);
	m_baseColor.a = 255;

	//allocate random garbage - no caching!
	int const arraySize = GetRandomValue(minArraySize, maxArraySize);
	m_garbageArray.resize(arraySize);

	for (int i = 0; i < arraySize; ++i)
	{
		m_garbageArray[i] = GetRandomValue(0, INT_MAX);
	}


	if (allCircles.empty() == false)
	{
		m_otherCircles.push_back(vsb::CreateSafePtr(allCircles[0]));
	}


}
