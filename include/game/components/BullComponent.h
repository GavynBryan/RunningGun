#pragma once
#include <map>
#include <memory>
#include <string_view>
#include <core/Component.h>
#include <core/PropertyDescriptor.h>
#include <core/animation/AnimationStateMachine.h>
#include <core/Vec2.h>
#include <core/events/MulticastDelegate.h>

#include <BullStates.h>

typedef std::unique_ptr<BullState> BullStatePtr;

class BullComponent :
	public Component
{
public:
	// ========== Serialization ==========
	static constexpr const char* TypeName = "bull";
	static constexpr PropertyDescriptor Properties[] = {}; // No configurable properties
	static std::unique_ptr<Component> Create(Entity& entity, GameServiceHost& context, std::string_view paramsJson);

private:
	std::map<std::string, BullStatePtr> States;
	AnimationStateMachine*				Animator;
	BullState*						CurrentState;

	uint8_t							Lives;

	Vec2							Offset1;
	Vec2							Offset2;

	Vec2							ProjectileOffset;
public:
									BullComponent(Entity& _entity, GameServiceHost& _context);
									~BullComponent();

	void							Start();
	void							Update();
	void							PostUpdate();

	void							Shoot();
	void							SwitchShootPositions();

	void							AddState(const std::string& _id, BullStatePtr _state);
	void							SwitchState(const std::string& _id);
	void							Damage();
	void							OnCollide(Entity& _other);

	// Events - GameMode subscribes to these
	MulticastDelegate<Entity*>		OnDied;
};
