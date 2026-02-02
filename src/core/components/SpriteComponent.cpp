#include <core/components/SpriteComponent.h>
#include <core/base/Actor.h>

SpriteComponent::SpriteComponent(Actor& entity, GameServiceHost& services)
	: ActorComponent(entity, services)
{
	// Register with the IRenderable registry (auto-unregisters on destruction)
	Registry = TryRegisterToInstanceRegistry<IRenderable>(services);
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

void SpriteComponent::SetRenderLayer(int layer)
{
	if (RenderLayer != layer)
	{
		RenderLayer = layer;
		if (Registry)
		{
			Registry->MarkDirty();
		}
	}
}

Transform2D SpriteComponent::GetTransform() const
{
	// Get transform directly from owning Actor
	return GetOwner()->GetTransform();
}

Rectf SpriteComponent::GetLocalBounds() const
{
	return Rectf(0.0f, 0.0f, Size.x, Size.y);
}
