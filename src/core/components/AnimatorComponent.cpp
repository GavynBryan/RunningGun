#include <core/components/AnimatorComponent.h>
#include <core/components/SpriteComponent.h>
#include <core/entity/Entity.h>
#include <core/framework/GameServiceHost.h>
#include <core/timing/TimeService.h>

AnimatorComponent::AnimatorComponent(Actor& entity, GameServiceHost& services)
	: ActorComponent(entity, services)
	, Time(&services.Get<TimeService>())
{
}

AnimatorComponent::~AnimatorComponent() = default;

void AnimatorComponent::Start()
{
	CachedSprite = Owner.GetComponent<SpriteComponent>();
}

void AnimatorComponent::Update()
{
	if (!CachedSprite || !Time) {
		return;
	}

	float deltaTime = Time->GetDeltaTime();
	Controller.Update(deltaTime, *CachedSprite);
}
