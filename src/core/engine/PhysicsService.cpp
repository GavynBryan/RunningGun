#include <core/engine/PhysicsService.h>

PhysicsService::PhysicsService()
	: PhysicsService(PhysicsConfig{})
{
}

PhysicsService::PhysicsService(const PhysicsConfig& config)
	: Gravity(config.Gravity),
	TerminalVelocity(config.TerminalVelocity),
	GroundLevel(config.GroundLevel),
	WorldBounds(config.WorldBounds),
	CollisionTree(config.WorldBounds)
{
}

void PhysicsService::SetWorldBounds(const Rectf& bounds)
{
	WorldBounds = bounds;
	CollisionTree = QuadTree(bounds);
}
