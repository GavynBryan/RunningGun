#pragma once
#include <core/Component.h>

class Physics;
class GameContext;

class PhysicsComponent :
	public Component
{
private:
	Physics &PhysContext;
public:
	PhysicsComponent(Entity& _entity, GameContext& _context);
	~PhysicsComponent();

	void Update();
	void PostUpdate();

	void OnCollide(Entity& _other);
};
