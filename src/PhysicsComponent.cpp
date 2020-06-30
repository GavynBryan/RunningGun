#include <PhysicsComponent.h>
#include <iostream>

PhysicsComponent::PhysicsComponent(Entity& _entity)
	:Component(_entity),
	physContext(*sEnvironment::Instance().getPhysics())
{
	
}


PhysicsComponent::~PhysicsComponent()
{
}

void PhysicsComponent::update()
{
	float gravity = physContext.getGravity().y;
	float currentVelocity = mEntity.getVelocity().y;
	if (currentVelocity < gravity) {
		float res = currentVelocity + gravity * (sEnvironment::Instance().deltaTime());
		res = std::min(res, gravity);
		mEntity.setVelocity(mEntity.getVelocity().x, res);
	}
	
}

void PhysicsComponent::postUpdate()
{
	auto position = mEntity.getPosition();

	position.x = std::min(position.x, 600.0f);
	position.x = std::max(position.x, 0.0f);
	//position.y = std::max(position.y, 0.0f);
	position.y = std::min(position.y, physContext.getGroundLevel());
	mEntity.setPosition(position);
}

void PhysicsComponent::onCollide(Entity& _other)
{

}