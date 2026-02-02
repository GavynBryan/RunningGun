#include <core/entity/Component.h>
#include <core/framework/GameServiceHost.h>
#include <core/logging/LoggingService.h>

ActorComponent::ActorComponent(Actor& owner, GameServiceHost& services)
	: Owner(owner)
	, Log(services.TryGet<LoggingService>())
{
}

ActorComponent::~ActorComponent() = default;

void ActorComponent::Start()
{
}

void ActorComponent::Update()
{
}

void ActorComponent::OnCollide(Actor& other)
{
}

void ActorComponent::Serialize(ISerializer& serializer) const
{
	// Default implementation - no properties to serialize
	// Overridden by SERIALIZABLE_COMPONENT macro in derived classes
}

void ActorComponent::Deserialize(const IDeserializer& deserializer)
{
	// Default implementation - no properties to deserialize
	// Overridden by SERIALIZABLE_COMPONENT macro in derived classes
}
