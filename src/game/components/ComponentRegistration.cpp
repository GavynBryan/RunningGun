#include <game/components/ComponentRegistration.h>
#include <core/ComponentRegistry.h>
#include <game/components/BullComponent.h>
#include <game/components/PatrolAIComponent.h>
#include <game/components/PhysicsComponent.h>
#include <game/components/PlayerComponent.h>
#include <game/components/ProjectileComponent.h>

void RegisterDefaultComponents(ComponentRegistry& registry)
{
	// Each component is self-describing - see the component headers for
	// serialization metadata (configurable JSON properties and their defaults).
	registry.Register(PlayerComponent::TypeName, PlayerComponent::Create);
	registry.Register(PhysicsComponent::TypeName, PhysicsComponent::Create);
	registry.Register(PatrolAIComponent::TypeName, PatrolAIComponent::Create);
	registry.Register(BullComponent::TypeName, BullComponent::Create);
	registry.Register(ProjectileComponent::TypeName, ProjectileComponent::Create);
}
