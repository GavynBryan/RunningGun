#pragma once
#include <SDL3/SDL.h>
#include <string>
#include <vector>
#include "Vec2.h"
#include "Rect.h"
#include "Sprite.h"
#include "AnimationListener.h"
#include "Component.h"

class GameContext;
class Camera;

enum ENTITY_TAG {
	player = 0,
	bullet,
	enemy_bullet,
	hazard,
	pickup
};

class Entity
{
protected:
	Vec2				Position;
	Vec2				Velocity;
	Vec2				Direction;

	Sprite				Sprite;

	std::vector<std::unique_ptr<Component>> Components;

	ENTITY_TAG			Tag;
	bool				Activated;

	std::unique_ptr<AnimationListener>	Animator;
	GameContext&							Context;

public:
	Entity(GameContext& _context, std::string _texture, float _width, float _height);
	~Entity();

	typedef std::unique_ptr<Entity> Ptr;

	void			AttachComponent(std::unique_ptr<Component> _comp);

	template <typename Comp>
	Comp*	GetComponent();

	virtual void		Start();
	virtual void		Update();
	virtual void		PostUpdate();
	void				Render(SDL_Renderer* _renderer, Camera* _camera = nullptr);

	void				StartComponents();
	void				UpdateComponents();
	void				PostUpdateComponents();

	void				SetTag(ENTITY_TAG _tag) { Tag = _tag; }
	void				Enable() { Activated = true; Start(); }
	void				Disable() { Activated = false; }

	void				SetPosition(float _x, float _y);
	void				SetPosition(Vec2 _pos);
	void				SetVelocity(float _x, float _y);
	void				SetVelocity(Vec2 _vel);
	void				SetDirection(Vec2 _dir);
	void				SetDirection(float _x, float _y);

	void				AssignAnimator(std::unique_ptr<AnimationListener> _animator);

	ENTITY_TAG			GetTag() { return Tag; }
	AnimationListener*	GetAnimator(){ return Animator.get(); }
	bool				IsEnabled() { return Activated; }
	bool				IsGrounded();
	void				OnCollide(Entity& _other);

	Vec2				GetPosition() { return Position; }
	Vec2				GetVelocity() { return Velocity; }
	Vec2				GetDirection() { return Direction; }
	Rectf				GetBoundingRect() { return Sprite.GetGlobalBounds(); }
	bool				Collision(Entity* _entity) { return _entity->GetBoundingRect().Intersects(GetBoundingRect()); }

	Sprite&				GetSprite() { return Sprite; }
	GameContext&		GetContext() { return Context; }
};

template<typename Comp>
Comp* Entity::GetComponent() {
	Comp* _result = nullptr;
	for (auto& _component : Components) {
		_result = dynamic_cast<Comp*>(_component.get());
		if (_result) break;
	}
	return _result;
}
