#pragma once
#include <map>
#include <core/entity/Component.h>
#include <core/math/Vec2.h>
#include <core/events/MulticastDelegate.h>

#include <BullStates.h>

typedef std::unique_ptr<BullState> BullStatePtr;

class ObjectPoolService;
class TimeService;
class AnimatorComponent;

class BullComponent :
	public ActorComponent
{
public:
	const char* GetName() const override { return "BullComponent"; }

private:
	ObjectPoolService& ObjectPool;
	TimeService& Time;
	std::map<std::string, BullStatePtr> States;
	AnimatorComponent*					Animator;
	BullState*						CurrentState;

	uint8_t							Lives;

	Vec2							Offset1;
	Vec2							Offset2;

	Vec2							ProjectileOffset;
public:
									BullComponent(Actor& _entity, GameServiceHost& _context);
									~BullComponent();

	void							Start();
	void							Update();

	void							Shoot();
	void							SwitchShootPositions();

	void							AddState(const std::string& _id, BullStatePtr _state);
	void							SwitchState(const std::string& _id);
	void							Damage();
	void							OnCollide(Actor& _other);

	// Service access for states
	TimeService& GetTime() { return Time; }

	// Events - GameMode subscribes to these
	MulticastDelegate<Actor*>		OnDied;
};
