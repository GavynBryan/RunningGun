#include <core/components/SpriteComponent.h>
#include <core/components/TransformComponent.h>
#include <core/entity/Entity.h>
#include <core/framework/GameServiceHost.h>
#include <core/rendering/RenderableRegistry.h>

SpriteComponent::SpriteComponent(Actor& entity, GameServiceHost& services)
	: ActorComponent(entity, services)
{
	Registry = services.TryGet<RenderableRegistry>();
	PairedTransform = entity.GetTransform();
	
	if (Registry && PairedTransform) {
		Registry->Register(PairedTransform, this);
	}
}

SpriteComponent::~SpriteComponent()
{
	if (Registry) {
		Registry->Unregister(this);
	}
}

void SpriteComponent::SetSourceRect(const Recti& rect)
{
	SourceRect = rect;
	HasSrcRect = true;
}

void SpriteComponent::SetSourceRect(int x, int y, int width, int height)
{
	SourceRect = Recti(x, y, width, height);
	HasSrcRect = true;
}

Rectf SpriteComponent::GetLocalBounds() const
{
	return Rectf(0.0f, 0.0f, Size.x, Size.y);
}
