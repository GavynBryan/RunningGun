#pragma once

#include <core/framework/ISystem.h>
#include <core/collision/CollisionService.h>
#include <vector>

class GameServiceHost;
class CollisionService;
class PhysicsService;

enum class CollisionPhase { Enter, Stay, Exit };

// Reads collision pairs from CollisionService and dispatches
// OnCollisionEnter/Stay/Exit and OnTriggerEnter/Stay/Exit events to RigidBody2D components.
class CollisionResponseSystem final : public ISystem
{
public:
	CollisionResponseSystem(GameServiceHost& services);

	void Update() override;

private:
	void DispatchCollisionEvents();
	void DispatchForPairs(const std::vector<CollisionPair>& pairs, CollisionPhase phase);

	GameServiceHost& Services;
	CollisionService& Collisions;
	PhysicsService& Physics;

	// Reusable buffers to avoid allocations
	std::vector<CollisionPair> Enters;
	std::vector<CollisionPair> Stays;
	std::vector<CollisionPair> Exits;
};
