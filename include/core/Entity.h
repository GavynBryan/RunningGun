#pragma once
#include <SDL3/SDL.h>
#include <string>
#include <vector>
#include "Vec2.h"
#include "Rect.h"
#include "Sprite.h"
#include "AnimationListener.h"
#include "Component.h"

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
	Vec2f				mPosition;
	Vec2f				mVelocity;
	Vec2f				mDirection;

	Sprite				mSprite;

	std::vector<std::unique_ptr<Component>> mComponents;

	ENTITY_TAG			mTag;
	bool				activated;
	bool				markedForDeath;

	std::unique_ptr<AnimationListener>	mAnimator;

public:
	Entity(std::string _texture, float _width, float _height);
	~Entity();

	typedef std::unique_ptr<Entity> Ptr;

	void			attachComponent(std::unique_ptr<Component> _comp);

	template <typename Comp>
	Comp*	getComponent();

	virtual void		start();
	virtual void		update();
	virtual void		postUpdate();
	void				render(SDL_Renderer* renderer);

	void				startComponents();
	void				updateComponents();
	void				postUpdateComponents();

	void				setTag(ENTITY_TAG _tag) { mTag = _tag; }
	void				enable() { activated = true; start(); }
	void				disable() { activated = false; }

	void				setPosition(float x, float y);
	void				setPosition(Vec2f _pos);
	void				setVelocity(float x, float y);
	void				setVelocity(Vec2f _vel);
	void				setDirection(Vec2f _dir);
	void				setDirection(float x, float y);

	void				assignAnimator(std::unique_ptr<AnimationListener> _animator);

	ENTITY_TAG			getTag() { return mTag; }
	AnimationListener*	getAnimator(){ return mAnimator.get(); }
	bool				isEnabled() { return activated; }
	bool				isMarkedForDeath() { return markedForDeath; }
	bool				isGrounded();
	void				onCollide(Entity& _other);

	Vec2f				getPosition() { return mPosition; }
	Vec2f				getVelocity() { return mVelocity; }
	Vec2f				getDirection() { return mDirection; }
	Rectf				getBoundingRect() { return mSprite.getGlobalBounds(); }
	bool				collision(Entity* _entity) { return _entity->getBoundingRect().intersects(getBoundingRect()); }

	Sprite&				getSprite() { return mSprite; }
};

template<typename Comp>
Comp* Entity::getComponent() {
	Comp* result = nullptr;
	for (auto& c : mComponents) {
		result = dynamic_cast<Comp*>(c.get());
		if (result) break;
	}
	return result;
}
