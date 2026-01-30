#pragma once
#include <core/Component.h>

class Physics;
class GameplayServices;

class PhysicsComponent :
	public Component
{
private:
	Physics &PhysContext;
public:
	PhysicsComponent(Entity& _entity, GameplayServices& _context);
	~PhysicsComponent();

	void Update();
	void PostUpdate();

	void OnCollide(Entity& _other);
};
