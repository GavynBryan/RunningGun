#pragma once
#include <map>
#include <core/Component.h>
#include <core/AnimationListener.h>
#include <core/ObjectPool.h>
#include <core/Vec2.h>

#include "BullStates.h"

typedef std::unique_ptr<BullState> BullStatePtr;

class BullComponent :
	public Component
{
private:
	std::map<std::string, BullStatePtr> States;
	AnimationListener*				Animator;
	BullState*						CurrentState;
	ObjectPool						Projectiles;

	uint8_t							Lives;
	bool							IsMarkedForDeath;

	Vec2							Offset1;
	Vec2							Offset2;

	Vec2							ProjectileOffset;
public:
									BullComponent(Entity& _entity);
									~BullComponent();

	void							Start();
	void							Update();
	void							PostUpdate();

	void							SetupProjectiles();
	void							Shoot();
	void							SwitchShootPositions();

	void							AddState(const std::string& _id, BullStatePtr _state);
	void							SwitchState(const std::string& _id);
	void							Damage();
	void							OnCollide(Entity& _other);
};
