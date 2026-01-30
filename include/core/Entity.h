#pragma once
#include <SDL3/SDL.h>
#include <string>
#include <vector>
#include <core/Vec2.h>
#include <core/Rect.h>
#include <core/Sprite.h>
#include <core/animation/AnimationStateMachine.h>
#include <core/Component.h>

class GameServiceHost;
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
	Vec2				Direction;

	Sprite				Sprite;

	std::vector<std::unique_ptr<Component>> Components;

	ENTITY_TAG			Tag;
	bool				Activated;

	std::unique_ptr<AnimationStateMachine>	Animator;
	GameServiceHost&					Services;

public:
	Entity(GameServiceHost& _services, std::string _texture, float _width, float _height);
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
	void				SetDirection(Vec2 _dir);
	void				SetDirection(float _x, float _y);

	void				AssignAnimator(std::unique_ptr<AnimationStateMachine> _animator);

	ENTITY_TAG			GetTag() const { return Tag; }
	AnimationStateMachine*	GetAnimator() { return Animator.get(); }
	const AnimationStateMachine*	GetAnimator() const { return Animator.get(); }
	bool				IsEnabled() const { return Activated; }
	void				OnCollide(Entity& _other);

	Vec2				GetPosition() const { return Position; }
	Vec2				GetDirection() const { return Direction; }
	Rectf				GetBoundingRect() const { return Sprite.GetGlobalBounds(); }
	bool				Collision(const Entity* _entity) const { return _entity->GetBoundingRect().Intersects(GetBoundingRect()); }

	Sprite&				GetSprite() { return Sprite; }
	const Sprite&			GetSprite() const { return Sprite; }
	GameServiceHost&		GetServices() { return Services; }
	float				GetWidth() const { return Sprite.GetGlobalBounds().w; }
	float				GetHeight() const { return Sprite.GetGlobalBounds().h; }
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
