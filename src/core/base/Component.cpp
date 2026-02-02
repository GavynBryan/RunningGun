#include <core/entity/Component.h>
#include <core/framework/GameServiceHost.h>
#include <core/logging/LoggingService.h>
#include <core/util/Json.h>

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

void ActorComponent::SerializeProperties(Json::Writer& writer) const
{
	// Default implementation - no properties to serialize
	// Overridden by COMPONENT macro in derived classes
}

void ActorComponent::DeserializeProperties(const simdjson::dom::element& json)
{
	// Default implementation - no properties to deserialize
	// Overridden by COMPONENT macro in derived classes
}
