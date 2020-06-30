#include <BullComponent.h>
#include <ProjectileComponent.h>



BullComponent::BullComponent(Entity& _entity)
	:Component(_entity),
	offset1(0,32),
	offset2(0,55),
	projectileOffset(offset1),
	lives(45),
	isMarkedForDeath(false)
{
	std::unique_ptr<BullDefaultState> _defaultState(new BullDefaultState(*this));

	addState("DefaultState", std::move(_defaultState));

	setupProjectiles();
	mEntity.setDirection(-1, 0);

}


BullComponent::~BullComponent()
{
}

void BullComponent::start()
{
	switchState("DefaultState");
	mAnimator = mEntity.getAnimator();
}

void BullComponent::update()
{
	if (!isMarkedForDeath) { 
		mCurrentState->update(); 
		mAnimator->playAnimation("default");
	}
}

void BullComponent::postUpdate()
{
	if (isMarkedForDeath) {
		//mEntity.disable();
	}

}

void BullComponent::setupProjectiles()
{
	for (int i = 0; i < 10; i++) {
		std::unique_ptr<Entity>_projectile(new Entity("sprites/waves.png", 32, 32));
		std::unique_ptr<ProjectileComponent> _projectileComponent(new ProjectileComponent(*_projectile, 400.0f, mEntity));

		_projectile->attachComponent(std::move(_projectileComponent));
		_projectile->setTag(enemy_bullet);
		mProjectiles.feedObject(std::move(_projectile));
	}
}

void BullComponent::shoot()
{
	auto p = mProjectiles.borrowObject();
	if (p != nullptr) {
		p->setPosition(mEntity.getPosition() + projectileOffset);
		switchShootPositions();
		mAnimator->playAnimation("shoot");
	}
}

void BullComponent::switchShootPositions() {
	if (projectileOffset == offset1) projectileOffset = offset2;
	else projectileOffset = offset1;
}

void BullComponent::addState(const std::string& _id, BullStatePtr _state)
{
	mStates.insert(std::make_pair(_id, std::move(_state)));
}

void BullComponent::switchState(const std::string& _id)
{
	auto nextState = mStates.find(_id);
	//if it can't access the state, we need to close it
	assert(nextState != mStates.end());
	if (nextState->second.get() == mCurrentState) { return; }
	if (mCurrentState != nullptr) {
		mCurrentState->exitState();
	}
	mCurrentState = nextState->second.get();
	mCurrentState->enterState();
}

void BullComponent::onCollide(Entity& _other)
{
	if (_other.getTag() == bullet) {
		damage();
	}
}

void BullComponent::damage() {
	if (!isMarkedForDeath) {
		mAnimator->playAnimation("damage");
		lives--;
		if (lives <= 0) {
			isMarkedForDeath = true;
			sEnvironment::Instance().Win();
			mAnimator->playAnimation("die");
		}
	}
}