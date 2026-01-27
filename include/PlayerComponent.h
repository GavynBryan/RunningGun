#pragma once
#include <core/ObjectPool.h>
#include <core/Component.h>
#include <core/Vec2.h>

#include "PlayerStates.h"

typedef std::unique_ptr<PlayerState> StatePtr;

constexpr auto bulletCoolDown = .3f;


class PlayerComponent :
	public Component
{
private:
	uint8_t lives;
	float playerSpeed;

	std::map<std::string, StatePtr> mStates;
	AnimationListener*				mAnimator;
	PlayerState*					mCurrentState;
	ObjectPool						mBullets;

	Vec2f							bulletOffset;
	float							lastShotTime;

public:
									PlayerComponent(Entity& _entity);
									~PlayerComponent();


	void							start();
	void							update();
	void							postUpdate();

	void							handleAnimations();

	void							setupBullets();
	void							shootBullet();
	int								getHealth() { return lives; }

	void							addState(const std::string& _id, StatePtr _state);
	void							switchState(const std::string& _id);

	void							orientDirection();
	void							handleInput();
	void							freeze();
	void							damage();

	void							onCollide(Entity& _other);
};
