#pragma once
#include "sEnvironment.h"
class ObjectPool
{
private:
	std::vector<Entity*> mPool;
public:
	ObjectPool();
	~ObjectPool();

	void feedObject(Entity::Ptr _obj);
	void clear();

	Entity* borrowObject();
};

