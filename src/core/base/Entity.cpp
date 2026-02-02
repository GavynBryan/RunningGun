#include <core/Entity.h>
#include <core/framework/GameServiceHost.h>
#include <core/rendering/RenderService.h>
#include <core/rendering/ResourceHandler.h>
#include <core/rendering/Camera.h>
#include <core/components/TransformComponent.h>
#include <core/components/SpriteComponent.h>

// Legacy constructor - maintains backward compatibility
Actor::Entity(GameServiceHost& _services, std::string _texture, float _width, float _height)
	: Position(0, 0)
	, Direction(1, 0)
	, Activated(true)
	, Services(_services)
{
	// Load resource handler from service (legacy path)
	auto& _handler = Services.Get<RenderService>().GetTextureHandler();
	EntitySprite.SetTexture(_handler.Get(_texture));
	EntitySprite.SetTextureRect(Recti(0, 0, static_cast<int>(_width), static_cast<int>(_height)));
}

// New minimal constructor
Actor::Entity(GameServiceHost& _services)
	: Position(0, 0)
	, Direction(1, 0)
	, Activated(true)
	, Services(_services)
{
}

Actor::~Entity()
{
}

void Actor::AttachComponent(std::unique_ptr<ActorComponent> _comp)
{
	Components.push_back(std::move(_comp));
	// Invalidate cache - will be rebuilt on next access
	CachedTransform = nullptr;
	CachedSprite = nullptr;
}

void Actor::CacheComponents()
{
	if (!CachedTransform) {
		CachedTransform = GetComponent<TransformComponent>();
	}
	if (!CachedSprite) {
		CachedSprite = GetComponent<SpriteComponent>();
	}
}

void Actor::StartComponents()
{
	if (Activated) {
		for (auto& _component : Components) {
			_component->Start();
		}
	}
}

void Actor::UpdateComponents()
{
	if (Activated) {
		for (auto& _component : Components) {
			_component->Update();
		}
	}
}

void Actor::Start()
{
	CacheComponents();
	StartComponents();
	
	// Sync position for collision (legacy path)
	if (!CachedTransform) {
		EntitySprite.SetPosition(Position);
	}
}

void Actor::Update()
{
	if (Activated) {
		UpdateComponents();

		if (Animator != nullptr) {
			// Animator still uses legacy Sprite for now
			// TODO: Create AnimatorComponent that works with SpriteComponent
			Animator->Update(EntitySprite);
		}
	}
}

void Actor::Render(SDL_Renderer* _renderer, Camera* _camera)
{
	// Only render via legacy path if not using component-based rendering
	if (Activated && !CachedSprite) {
		EntitySprite.SetPosition(GetPosition());
		EntitySprite.Render(_renderer, _camera);
	}
}

void Entity::SetPosition(float _x, float _y)
{
	CacheComponents();
	if (CachedTransform) {
		CachedTransform->SetPosition(_x, _y);
	}
	else {
		Position.x = _x;
		Position.y = _y;
		EntitySprite.SetPosition(Position);
	}
}

void Entity::SetPosition(Vec2 _pos)
{
	CacheComponents();
	if (CachedTransform) {
		CachedTransform->SetPosition(_pos);
	}
	else {
		Position = _pos;
		EntitySprite.SetPosition(Position);
	}
}

Vec2 Entity::GetPosition() const
{
	const_cast<Entity*>(this)->CacheComponents();
	if (CachedTransform) {
		return CachedTransform->GetPosition();
	}
	return Position;
}

void Entity::SetDirection(Vec2 _dir)
{
	CacheComponents();
	if (CachedTransform) {
		CachedTransform->SetDirection(_dir);
	}
	else {
		Direction = _dir;
	}
}

void Entity::SetDirection(float _x, float _y)
{
	CacheComponents();
	if (CachedTransform) {
		CachedTransform->SetDirection(_x, _y);
	}
	else {
		Direction.x = _x;
		Direction.y = _y;
	}
}

Vec2 Entity::GetDirection() const
{
	const_cast<Entity*>(this)->CacheComponents();
	if (CachedTransform) {
		return CachedTransform->GetDirection();
	}
	return Direction;
}

void Entity::AssignAnimator(std::unique_ptr<AnimationStateMachine> _animator)
{
	Animator = std::move(_animator);
}

void Entity::OnCollide(Entity& _other)
{
	if (Activated) {
		for (auto& _component : Components) {
			_component->OnCollide(_other);
		}
	}
}

float Entity::GetWidth() const
{
	const_cast<Entity*>(this)->CacheComponents();
	if (CachedSprite) {
		return CachedSprite->GetWidth();
	}
	return EntitySprite.GetGlobalBounds().w;
}

float Entity::GetHeight() const
{
	const_cast<Entity*>(this)->CacheComponents();
	if (CachedSprite) {
		return CachedSprite->GetHeight();
	}
	return EntitySprite.GetGlobalBounds().h;
}

void Entity::SetFlipX(bool flip)
{
	CacheComponents();
	if (CachedSprite) {
		CachedSprite->SetFlipX(flip);
	}
	else {
		EntitySprite.SetFlipX(flip);
	}
}

bool Entity::GetFlipX() const
{
	const_cast<Entity*>(this)->CacheComponents();
	if (CachedSprite) {
		return CachedSprite->GetFlipX();
	}
	// Legacy Sprite doesn't have GetFlipX, so return false
	return false;
}
