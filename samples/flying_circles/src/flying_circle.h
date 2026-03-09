//
// Created by Alexander on 12.01.2026.
//

#pragma once

#include "raylib.h"
#include "vsb/types.h"
#include "vsb/objects/managed_object.h"
#include "vsb/objects/safe_ptr.h"
#include "vsb/objects/ptr.h"
#include "vsb/containers/vsb_vector.h"

class FlyingCircle;
using FlyingCirclePtr = vsb::Ptr<FlyingCircle>;


class FlyingCircle : public vsb::ManagedObject
{
	USE_VSB_TYPES_INSIDE;

public:


	FlyingCircle(float lifetime, int minArraySize, int maxArraySize, const std::vector<FlyingCirclePtr>& allCircles);
	~FlyingCircle() override = default;

	void* operator new(std::size_t size);
	void operator delete(void* ptr, std::size_t size) noexcept;



	void Update(float dt);
	void PostUpdate();
	void Destroy();

	void Draw() const;
	void DrawQuad() const;

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


	static constexpr float FadeInDuration = 2.0f;
	static constexpr float FadeOutDuration = 5.0f;

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

	vsb::vector<int> m_garbageArray {};
	vsb::vector<vsb::SafePtr<FlyingCircle>> m_otherCircles {};
};
