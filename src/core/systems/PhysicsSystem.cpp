#include <core/systems/PhysicsSystem.h>
#include <core/engine/GameServiceHost.h>
#include <core/engine/PhysicsService.h>
#include <core/engine/WorldService.h>
#include <core/World.h>

PhysicsSystem::PhysicsSystem(GameServiceHost& services)
	: Services(services)
{
}

void PhysicsSystem::Update()
{
	auto& world = Services.Get<WorldService>().GetWorld();
	const auto& entities = world.GetEntities();
	if (entities.empty()) {
		return;
	}
	UpdateEntityCollisions(entities);
}

void PhysicsSystem::UpdateEntityCollisions(const std::vector<Entity::Ptr>& entities)
{
	auto& physics = Services.Get<PhysicsService>();
	auto& collisionTree = physics.GetCollisionTree();
	auto& candidates = physics.GetCollisionCandidates();

	collisionTree.Clear();
	for (const auto& entity : entities) {
		if (entity->IsEnabled()) {
			collisionTree.Insert(entity.get());
		}
	}

	for (const auto& entity : entities) {
		if (!entity->IsEnabled()) {
			continue;
		}
		candidates.clear();
		collisionTree.Query(entity->GetBoundingRect(), candidates);
		for (auto* other : candidates) {
			if (other == entity.get()) {
				continue;
			}
			if (!other->IsEnabled()) {
				continue;
			}
			if (entity.get() < other && entity->Collision(other)) {
				entity->OnCollide(*other);
				other->OnCollide(*entity.get());
			}
		}
	}
}
