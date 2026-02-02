#include <core/systems/collision/CollisionResponseSystem.h>
#include <core/services/framework/GameServiceHost.h>
#include <core/services/collision/CollisionService.h>
#include <core/services/physics/PhysicsService.h>
#include <core/components/RigidBody2DComponent.h>

CollisionResponseSystem::CollisionResponseSystem(GameServiceHost& services)
	: Services(services)
	, Collisions(services.Get<CollisionService>())
	, Physics(services.Get<PhysicsService>())
{
}

void CollisionResponseSystem::Update()
{
	DispatchCollisionEvents();
}

void CollisionResponseSystem::DispatchCollisionEvents()
{
	// Get collision state changes
	Collisions.GetCollisionEnters(Enters);
	Collisions.GetCollisionStays(Stays);
	Collisions.GetCollisionExits(Exits);

	// Dispatch events
	DispatchForPairs(Enters, CollisionPhase::Enter);
	DispatchForPairs(Stays, CollisionPhase::Stay);
	DispatchForPairs(Exits, CollisionPhase::Exit);
}

void CollisionResponseSystem::DispatchForPairs(const std::vector<CollisionPair>& pairs, CollisionPhase phase)
{
	for (const auto& pair : pairs) {
		RigidBody2DComponent* bodyA = Physics.GetRigidBody(pair.A);
		RigidBody2DComponent* bodyB = Physics.GetRigidBody(pair.B);

		// Skip if either body was destroyed
		if (!bodyA || !bodyB) {
			continue;
		}

		// Dispatch events based on phase and trigger status
		if (pair.IsTriggerCollision) {
			// Trigger events
			switch (phase) {
				case CollisionPhase::Enter:
					bodyA->OnTriggerEnter.Broadcast(*bodyA, *bodyB);
					bodyB->OnTriggerEnter.Broadcast(*bodyB, *bodyA);
					break;
				case CollisionPhase::Stay:
					bodyA->OnTriggerStay.Broadcast(*bodyA, *bodyB);
					bodyB->OnTriggerStay.Broadcast(*bodyB, *bodyA);
					break;
				case CollisionPhase::Exit:
					bodyA->OnTriggerExit.Broadcast(*bodyA, *bodyB);
					bodyB->OnTriggerExit.Broadcast(*bodyB, *bodyA);
					break;
			}
		} else {
			// Collision events
			switch (phase) {
				case CollisionPhase::Enter:
					bodyA->OnCollisionEnter.Broadcast(*bodyA, *bodyB);
					bodyB->OnCollisionEnter.Broadcast(*bodyB, *bodyA);
					break;
				case CollisionPhase::Stay:
					bodyA->OnCollisionStay.Broadcast(*bodyA, *bodyB);
					bodyB->OnCollisionStay.Broadcast(*bodyB, *bodyA);
					break;
				case CollisionPhase::Exit:
					bodyA->OnCollisionExit.Broadcast(*bodyA, *bodyB);
					bodyB->OnCollisionExit.Broadcast(*bodyB, *bodyA);
					break;
			}
		}
	}
}
