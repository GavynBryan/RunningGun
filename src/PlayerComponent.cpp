#include <PlayerComponent.h>
#include <PhysicsComponent.h>
#include <ProjectileComponent.h>
#include <utility.h>
#include <core/Entity.h>
#include <core/InputManager.h>

PlayerComponent::PlayerComponent(Entity& _entity)
	:Component(_entity),
	playerSpeed(350),
	lives(5),
	bulletOffset(32, 18),
	mAnimator(nullptr),
	mCurrentState(nullptr),
	lastShotTime(0)
{
	//initialize states
	std::unique_ptr<defaultPlayerState>		_defaultState(new defaultPlayerState(*this));
	std::unique_ptr<damagePlayerState>		_damageState(new damagePlayerState(*this));
	std::unique_ptr<deadPlayerState>		_deadState(new deadPlayerState(*this));
	std::unique_ptr<victoryPlayerState>		_victoryState(new victoryPlayerState(*this));

	addState("DefaultState", std::move(_defaultState));
	addState("DamageState", std::move(_damageState));
	addState("DeadState", std::move(_deadState));
	addState("VictoryState", std::move(_victoryState));

	//set initial direction (right)
	mEntity.setDirection(1, 0);
	setupBullets();
}


PlayerComponent::~PlayerComponent()
{
}

void PlayerComponent::start()
{
	switchState("DefaultState");
	lastShotTime = 0;

	mAnimator = mEntity.getAnimator();
}

void PlayerComponent::update()
{
	mCurrentState->update();
	handleAnimations();
}

void PlayerComponent::postUpdate()
{
	mCurrentState->postUpdate();
	orientDirection();
}

void PlayerComponent::handleAnimations()
{
	if (mEntity.getDirection().x > 0) {
		if (mEntity.getVelocity().x != 0)
			mAnimator->playAnimation("walkright");
		else mAnimator->playAnimation("right");
	} else{
		if (mEntity.getVelocity().x != 0)
			mAnimator->playAnimation("walkleft");
		else mAnimator->playAnimation("left");
	}

}
//load up the bullets into the object pool
void PlayerComponent::setupBullets()
{
	for (int i = 0; i < 20; i++) {
		std::unique_ptr<Entity>_projectile(new Entity("sprites/bullet.png", 16, 16));
		std::unique_ptr<ProjectileComponent> _projectileComponent(new ProjectileComponent(*_projectile, 400.0f, mEntity));

		_projectile->attachComponent(std::move(_projectileComponent));
		_projectile->setTag(bullet);
		mBullets.feedObject(std::move(_projectile));
	}
}

void PlayerComponent::shootBullet()
{
	//update the shooting cooldown
	auto currentTime = sEnvironment::Instance().getElapsedTime();
	if (currentTime - bulletCoolDown > lastShotTime) {
		lastShotTime = currentTime;
		//borrow bullet from object pool
		auto b = mBullets.borrowObject();

		//set position based off of player's direction
		auto position = mEntity.getPosition();
		float xOrigin = position.x +(mEntity.getBoundingRect().width / 2);
		position.x = xOrigin + (mEntity.getDirection().x * bulletOffset.x);
		position.y += bulletOffset.y;
		if (b != nullptr) {
			b->setPosition(position);
		}
		if (mEntity.getVelocity().x == 0) {
			if(mEntity.getDirection().x > 0)
				mAnimator->playAnimation("shootright");
			else
				mAnimator->playAnimation("shootleft");
		}
	}
}

void PlayerComponent::addState(const std::string& _id, StatePtr _state)
{
	mStates.insert(std::make_pair(_id, std::move(_state)));
}

void PlayerComponent::switchState(const std::string& _id)
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

void PlayerComponent::orientDirection()
{
	//the direction would just be a normalized version of the last non-zero x velocity
	if (mEntity.getVelocity().x != 0) {
		Vec2 direction = mEntity.getVelocity();
		direction.y = 0;
		mEntity.setDirection(VectorMath::Normalize(direction));
	}
}

void PlayerComponent::handleInput()
{
	auto& input = InputManager::Instance();

	Vec2 _velocity = Vec2(0, mEntity.getVelocity().y);

	// Movement - use held state for continuous movement
	if (input.isKeyHeld(SDL_SCANCODE_LEFT)) {
		_velocity = Vec2(-playerSpeed, mEntity.getVelocity().y);
	}
	if (input.isKeyHeld(SDL_SCANCODE_RIGHT)) {
		_velocity = Vec2(playerSpeed, mEntity.getVelocity().y);
	}

	// Jump - use pressed state to only jump on initial press
	if (input.isKeyPressed(SDL_SCANCODE_Z)) {
		if (mEntity.isGrounded()) {
			_velocity = Vec2(mEntity.getVelocity().x, -500);
		}
	}

	// Fire - use pressed state to only fire on initial press (not held)
	if (input.isKeyPressed(SDL_SCANCODE_X)) {
		shootBullet();
	}

	mEntity.setVelocity(_velocity);
}

void PlayerComponent::freeze()
{
	mEntity.setVelocity(0, mEntity.getVelocity().y);
}

void PlayerComponent::onCollide(Entity& _other)
{
	if (_other.getTag() == hazard || _other.getTag() == enemy_bullet) {
		mCurrentState->damage();
	}
}

void PlayerComponent::damage()
{
	lives--;
	if (mEntity.getDirection().x > 0)
		mAnimator->playAnimation("damageright");
	else
		mAnimator->playAnimation("damageleft");
	if (lives <= 0) {
		switchState("DeadState");
	}
}
