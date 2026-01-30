#include <core/engine/PhysicsService.h>
#include <core/World.h>
#include <core/engine/WorldService.h>

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

void PhysicsService::Update()
{
	auto& world = GetHost().Get<WorldService>().GetWorld();
	const auto& entities = world.GetEntities();
	if (entities.empty()) {
		return;
	}
	UpdateEntityCollisions(entities);
}

void PhysicsService::UpdateEntityCollisions(const std::vector<Entity::Ptr>& entities)
{
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
