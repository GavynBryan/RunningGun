#pragma once
#include <core/Component.h>
#include <core/sEnvironment.h>

class PhysicsComponent :
	public Component
{
private:
	sPhysics &physContext;
public:
	PhysicsComponent(Entity& _entity);
	~PhysicsComponent();

	void update();
	void postUpdate();

	void onCollide(Entity& _other);
};

