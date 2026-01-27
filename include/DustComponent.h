#pragma once
#include <core/Component.h>

class DustComponent
	:public Component
{
private:
	float Speed;
	float LifeSpan = 4.0f;
	float SpawnTime;
public:
	DustComponent(Entity& _entity, float _speed);
	~DustComponent();
};

