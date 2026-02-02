#include <core/entity/Component.h>
#include <core/framework/GameServiceHost.h>
#include <core/logging/LoggingService.h>

ActorComponent::ActorComponent(Actor& owner, GameServiceHost& services)
	: Owner(&owner)
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
