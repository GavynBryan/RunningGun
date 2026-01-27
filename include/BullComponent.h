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
	std::map<std::string, BullStatePtr> mStates;
	AnimationListener*				mAnimator;
	BullState*						mCurrentState;
	ObjectPool						mProjectiles;

	uint8_t							lives;
	bool							isMarkedForDeath;

	Vec2f							offset1;
	Vec2f							offset2;

	Vec2f							projectileOffset;
public:
									BullComponent(Entity& _entity);
									~BullComponent();

	void							start();
	void							update();
	void							postUpdate();

	void							setupProjectiles();
	void							shoot();
	void							switchShootPositions();

	void							addState(const std::string& _id, BullStatePtr _state);
	void							switchState(const std::string& _id);
	void							damage();
	void							onCollide(Entity& _other);
};
