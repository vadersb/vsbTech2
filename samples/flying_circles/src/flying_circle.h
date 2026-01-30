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


	FlyingCircle(f32 lifetime, i32 minArraySize, i32 maxArraySize, const std::vector<FlyingCirclePtr>& allCircles);
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


	static constexpr float FadeInDuration = 1.0f;
	static constexpr float FadeOutDuration = 1.0f;

	static constexpr int MinOtherCircles = 2;
	static constexpr int MaxOtherCircles = 32;


	f32 m_x;
	f32 m_y;
	f32 m_velX;
	f32 m_velY;

	f32 m_radius;

	Color m_baseColor {};

	f32 m_lifetime;
	f32 m_age {0.0f};

	vsb::vector<i32> m_garbageArray {};
	vsb::vector<vsb::SafePtr<FlyingCircle>> m_otherCircles {};
};
