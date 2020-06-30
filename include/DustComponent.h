#pragma once
#include <core/Component.h>

class DustComponent
	:public Component
{
private:
	float speed;
	float lifespan = 4.0f;
	float spawnTime;
public:
	DustComponent(Entity& _entity, float _speed);
	~DustComponent();
};

