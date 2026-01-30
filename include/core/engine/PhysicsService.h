#pragma once

#include <core/Vec2.h>
#include <core/engine/IService.h>

class PhysicsService final : public IService
{
public:
	PhysicsService();

	float GetGroundLevel() const { return GroundLevel; }
	Vec2 GetGravity() const { return Gravity; }

private:
	Vec2 Gravity;
	float GroundLevel = 0.0f;
};
