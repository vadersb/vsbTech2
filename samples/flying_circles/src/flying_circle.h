//
// Created by Alexander on 12.01.2026.
//

#pragma once

#include "raylib.h"
#include "vsb/objects/managed_object.h"
#include "basic_allocator.h"

class FlyingCircle : public vsb::ManagedObjectDefault
{
public:


	FlyingCircle(float lifetime, int minArraySize, int maxArraySize, const std::vector<FlyingCircle*>& allCircles);

	static void* operator new(std::size_t size);
	static void operator delete(void* ptr, std::size_t size) noexcept;



	void Update(float dt);
	void Destroy();

	void Draw() const;

	[[nodiscard]] int GetOtherCirclesCount() const;


	[[nodiscard]] bool IsDead() const
	{
		return m_age >= m_lifetime;
	}

private:

	[[nodiscard]] float CalculateAlpha() const;

	[[nodiscard]] float GetCurRadius() const;

	[[nodiscard]] float GetRadiusDeltaFromGarbageArray() const;

	void UpdateOtherCirclesList();


	static constexpr float FadeInDuration = 1.0f;
	static constexpr float FadeOutDuration = 1.0f;

	static constexpr int MinOtherCircles = 2;
	static constexpr int MaxOtherCircles = 32;


	float m_x;
	float m_y;
	float m_velX;
	float m_velY;

	float m_radius;

	Color m_baseColor {};

	float m_lifetime;
	float m_age {0.0f};

	flying_circles::PooledVector<int> m_garbageArray {};
	flying_circles::PooledVector<vsb::SafePtr<FlyingCircle>> m_otherCircles {};
};
