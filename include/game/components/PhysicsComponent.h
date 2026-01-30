#pragma once
#include <core/Component.h>

class GameServiceHost;
class PhysicsService;

class PhysicsComponent :
	public Component
{
private:
	PhysicsService& PhysContext;
public:
	PhysicsComponent(Entity& _entity, GameServiceHost& _context);
	~PhysicsComponent();

	void Update();
	void PostUpdate();

	void OnCollide(Entity& _other);
};
