#pragma once
#include <core/ObjectPool.h>
#include <core/Component.h>
#include <core/Vec2.h>

#include "PlayerStates.h"

typedef std::unique_ptr<PlayerState> StatePtr;

constexpr auto BulletCoolDown = .3f;


class PlayerComponent :
	public Component
{
private:
	uint8_t Lives;
	float PlayerSpeed;

	std::map<std::string, StatePtr> States;
	AnimationListener*				Animator;
	PlayerState*					CurrentState;
	ObjectPool						Bullets;

	Vec2							BulletOffset;
	float							LastShotTime;

public:
									PlayerComponent(Entity& _entity);
									~PlayerComponent();


	void							Start();
	void							Update();
	void							PostUpdate();

	void							HandleAnimations();

	void							SetupBullets();
	void							ShootBullet();
	int								GetHealth() { return Lives; }

	void							AddState(const std::string& _id, StatePtr _state);
	void							SwitchState(const std::string& _id);

	void							OrientDirection();
	void							HandleInput();
	void							Freeze();
	void							Damage();

	void							OnCollide(Entity& _other);
};
