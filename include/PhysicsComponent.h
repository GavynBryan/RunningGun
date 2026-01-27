#pragma once
#include <core/Component.h>
#include <core/Environment.h>

class PhysicsComponent :
	public Component
{
private:
	Physics &PhysContext;
public:
	PhysicsComponent(Entity& _entity);
	~PhysicsComponent();

	void Update();
	void PostUpdate();

	void OnCollide(Entity& _other);
};

