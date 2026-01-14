//
// Created by Alexander on 12.01.2026.
//

#include "flying_circle.h"
#include "flying_circles_settings.h"
#include "flying_circles_utils.h"
#include "raylib.h"
#include "vsb/objects/safe_ptr.h"
#include "basic_memory_pool.h"
#include "../../../cmake-build-debug-visual-studio/_deps/raylib-src/src/rlgl.h"


FlyingCircle::FlyingCircle(float lifetime, int minArraySize, int maxArraySize, const std::vector<FlyingCircle*>& allCircles) :
	m_x(fc_utils::GetRandomFloat(0.0f, params::WindowWidth)),
	m_y(fc_utils::GetRandomFloat(0.0f, params::WindowHeight)),
	m_velX((fc_utils::GetRandomFloat01() - 0.5f) * 100.0f),
	m_velY((fc_utils::GetRandomFloat01() - 0.5f) * 100.0f),
	m_radius(5.0f + fc_utils::GetRandomFloat01() * 15.0f),
	m_lifetime(lifetime)
{
	m_baseColor.r = static_cast<unsigned char>(GetRandomValue(100, 256));
	m_baseColor.g = static_cast<unsigned char>(GetRandomValue(100, 256));
	m_baseColor.b = static_cast<unsigned char>(GetRandomValue(100, 256));
	m_baseColor.a = 255;

	//allocate random garbage - no caching!
	int const arraySize = GetRandomValue(minArraySize, maxArraySize);
	m_garbageArray.resize(arraySize);

	for (int i = 0; i < arraySize; ++i)
	{
		m_garbageArray[i] = GetRandomValue(0, INT_MAX);
	}


	const int otherCirclesCount = std::min(static_cast<int>(allCircles.size()),
	                                 GetRandomValue(MinOtherCircles, MaxOtherCircles));

	for (int i = 0; i < otherCirclesCount; i++)
	{
		const int otherCircleIndex = GetRandomValue(0, static_cast<int>(allCircles.size()));

		if (otherCircleIndex >= static_cast<int>(allCircles.size()))
		{
			continue;
		}

		FlyingCircle* otherCircle = allCircles[otherCircleIndex];
		m_otherCircles.push_back(vsb::CreateSafePtr(otherCircle));
	}
}


void* FlyingCircle::operator new(std::size_t size)
{
	return flying_circles::BasicMemoryPool::Allocate(size);
}


void FlyingCircle::operator delete(void* ptr, std::size_t size) noexcept
{
	flying_circles::BasicMemoryPool::Deallocate(ptr, size);
}


void FlyingCircle::Update(float dt)
{
	m_age += dt;
	m_x += m_velX * dt;
	m_y += m_velY * dt;

	// Bounce off walls
	if (m_x < 0 || m_x > params::WindowWidth)
	{
		m_velX = -m_velX;
		m_x = std::clamp(m_x, 0.0f, static_cast<float>(params::WindowWidth));
	}
	if (m_y < 0 || m_y > params::WindowHeight)
	{
		m_velY = -m_velY;
		m_y = std::clamp(m_y, 0.0f, static_cast<float>(params::WindowHeight));
	}


	UpdateOtherCirclesList();
}


void FlyingCircle::Destroy()
{
	ScheduleForDestruction();
}


void FlyingCircle::Draw() const
{
	float alpha = CalculateAlpha();
	Color color(m_baseColor.r, m_baseColor.g, m_baseColor.b, static_cast<unsigned char>(alpha * 255));
	DrawCircle(static_cast<int>(m_x), static_cast<int>(m_y), GetCurRadius(), color);
}


void FlyingCircle::DrawQuad() const
{
	if (IsDead())
	{
		return;
	}

	const float alpha = CalculateAlpha();
	const Color color(m_baseColor.r, m_baseColor.g, m_baseColor.b, static_cast<unsigned char>(alpha * 255));

	rlColor4ub(color.r, color.g, color.b, color.a);
	const float r = GetCurRadius();

	rlTexCoord2f(0.0f, 1.0f); rlVertex2f(m_x - r, m_y + r); //bottom-left
	rlTexCoord2f(1.0f, 1.0f); rlVertex2f(m_x + r, m_y + r); //bottom-right
	rlTexCoord2f(1.0f, 0.0f); rlVertex2f(m_x + r, m_y - r); //top-right
	rlTexCoord2f(0.0f, 0.0f); rlVertex2f(m_x - r, m_y - r); //top-left
}


int FlyingCircle::GetOtherCirclesCount() const
{
	return static_cast<int>(m_otherCircles.size());
}


float FlyingCircle::CalculateAlpha() const
{
	float timeUntilDeath = m_lifetime - m_age;

	// Fade in: 0 to 1 over first second
	if (m_age < FadeInDuration)
	{
		return m_age / FadeInDuration;
	}

	// Fade out: 1 to 0 over last second
	if (timeUntilDeath < FadeOutDuration)
	{
		return timeUntilDeath / FadeOutDuration;
	}

	// Fully visible in between
	return 1.0f;
}


float FlyingCircle::GetCurRadius() const
{
	return m_radius + static_cast<float>(m_otherCircles.size()) + GetRadiusDeltaFromGarbageArray();
}


float FlyingCircle::GetRadiusDeltaFromGarbageArray() const
{
	if (m_garbageArray.empty())
	{
		return -10.0f;
	}

	float result = m_garbageArray[0] % 2 == 0 ? +3.0f : -5.0f;

	if (m_garbageArray.size() > 500)
	{
		result += 3.0f;
	}

	return result;
}


void FlyingCircle::UpdateOtherCirclesList()
{
	bool hasRemovals = false;

	for (auto& otherCircle : m_otherCircles)
	{
		if (otherCircle.Validate() == false)
		{
			otherCircle.Reset();
			hasRemovals = true;
		}
	}

	if (hasRemovals)
	{
		std::erase_if(m_otherCircles, [](const auto &ptr) { return ptr.IsEmpty(); });
	}
}
