#include <core/engine/PhysicsService.h>
#include <core/World.h>
#include <core/engine/WorldService.h>

PhysicsService::PhysicsService()
	: Gravity(0.0f, 1000.0f),
	GroundLevel(460.0f),
	CollisionTree(Rectf(0.0f, 0.0f, 800.0f, 600.0f))
{
	CollisionCandidates.reserve(32);
}

void PhysicsService::Update()
{
	auto& world = GetHost().Get<WorldService>().GetWorld();
	const auto& entities = world.GetEntities();
	if (entities.empty()) {
		return;
	}

	if (CollisionCandidates.capacity() < entities.size()) {
		CollisionCandidates.reserve(entities.size());
	}

	CollisionTree.Clear();
	for (const auto& entity : entities) {
		if (entity->IsEnabled()) {
			CollisionTree.Insert(entity.get());
		}
	}

	for (const auto& entity : entities) {
		if (!entity->IsEnabled()) {
			continue;
		}
		CollisionCandidates.clear();
		CollisionTree.Query(entity->GetBoundingRect(), CollisionCandidates);
		for (auto* other : CollisionCandidates) {
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
