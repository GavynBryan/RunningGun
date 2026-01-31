#pragma once

#include <core/QuadTree.h>
#include <core/Vec2.h>
#include <vector>

class Entity;

struct PhysicsConfig
{
	Vec2 Gravity = Vec2(0.0f, 1000.0f);
	float TerminalVelocity = 800.0f;
	float GroundLevel = 460.0f;
	Rectf WorldBounds = Rectf(0.0f, 0.0f, 800.0f, 600.0f);
};

class PhysicsService final
{
public:
	PhysicsService();
	explicit PhysicsService(const PhysicsConfig& config);

	float GetGroundLevel() const { return GroundLevel; }
	Vec2 GetGravity() const { return Gravity; }
	float GetTerminalVelocity() const { return TerminalVelocity; }
	const Rectf& GetWorldBounds() const { return WorldBounds; }

	void SetGravity(const Vec2& gravity) { Gravity = gravity; }
	void SetTerminalVelocity(float velocity) { TerminalVelocity = velocity; }
	void SetGroundLevel(float level) { GroundLevel = level; }
	void SetWorldBounds(const Rectf& bounds);

	QuadTree& GetCollisionTree() { return CollisionTree; }
	std::vector<Entity*>& GetCollisionCandidates() { return CollisionCandidates; }

private:
	Vec2 Gravity;
	float TerminalVelocity;
	float GroundLevel;
	Rectf WorldBounds;
	QuadTree CollisionTree;
	std::vector<Entity*> CollisionCandidates;
};
