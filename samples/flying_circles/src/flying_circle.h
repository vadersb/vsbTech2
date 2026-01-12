//
// Created by Alexander on 12.01.2026.
//

#pragma once

#include "flying_circles_settings.h"
#include "raylib.h"
#include "vsb/objects/managed_object.h"


class FlyingCircle : public vsb::ManagedObjectDefault
{
public:


	FlyingCircle(float lifetime, int minArraySize, int maxArraySize, const std::vector<FlyingCircle*>& allCircles);


	[[nodiscard]] bool IsDead() const
	{
		return m_age >= m_lifetime;
	}

private:

	float m_x;
	float m_y;
	float m_velX;
	float m_velY;

	float m_radius;

	Color m_baseColor {};

	float m_lifetime;
	float m_age {0.0f};

	std::vector<int> m_garbageArray {};
	std::vector<vsb::SafePtr<FlyingCircle>> m_otherCircles {};

};
