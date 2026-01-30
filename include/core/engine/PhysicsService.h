#pragma once

#include <core/QuadTree.h>
#include <core/Vec2.h>
#include <core/engine/IService.h>
#include <vector>

class PhysicsService final : public IService
{
public:
	PhysicsService();

	void Update() override;

	float GetGroundLevel() const { return GroundLevel; }
	Vec2 GetGravity() const { return Gravity; }

private:
	Vec2 Gravity;
	float GroundLevel = 0.0f;
	QuadTree CollisionTree;
	std::vector<Entity*> CollisionCandidates;
};
