#include <PatrolAIComponent.h>
#include <core/Entity.h>
#include <core/sEnvironment.h>
#include <core/AnimationListener.h>

PatrolAIComponent::PatrolAIComponent(Entity& _entity, float _speed)
	:Component(_entity),
	moveSpeed(_speed)
{
}


PatrolAIComponent::~PatrolAIComponent()
{
}

void PatrolAIComponent::start()
{
	markedForDeath = false;
	lives = 2;
	mAnimator = mEntity.getAnimator();
	lastTurnAround = sEnvironment::Instance().getElapsedTime();
}

void PatrolAIComponent::update()
{
	if (mEntity.isGrounded()) {
		sf::Vector2f patrolVelocity = mEntity.getDirection();
		patrolVelocity.y = mEntity.getVelocity().y;
		patrolVelocity.x *= moveSpeed;
		mEntity.setVelocity(patrolVelocity);
		float currentTime = sEnvironment::Instance().getElapsedTime();

		if (currentTime - interval > lastTurnAround) {
			changeDirection();
			lastTurnAround = currentTime;
		}
	}
	else { mEntity.setVelocity(0, mEntity.getVelocity().y); }
}

void PatrolAIComponent::postUpdate()
{
	if (markedForDeath) {
		mEntity.disable();
		return;
	}
	if (mEntity.getDirection().x == 1) {
		mAnimator->playAnimation("left");
	}
	else {
		mAnimator->playAnimation("right");
	}
}

void PatrolAIComponent::changeDirection()
{
	mEntity.setDirection(mEntity.getDirection() * -1.0f);
}

void PatrolAIComponent::onCollide(Entity& _other)
{
	if (_other.getTag() == bullet) {
		damage();
	}
}

void PatrolAIComponent::damage()
{
	lives--;
	if (lives <= 0) {
		markedForDeath = true;
		return;
	}
	if (mEntity.getDirection().x == 1) {
		mAnimator->playAnimation("damageleft");
	}
	else {
		mAnimator->playAnimation("damageright");
	}
}

//absolutely useless
void PatrolAIComponent::die()
{
	mEntity.disable();
}