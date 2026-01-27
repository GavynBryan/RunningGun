#pragma once
#include "Environment.h"
class ObjectPool
{
private:
	std::vector<Entity*> Pool;
public:
	ObjectPool();
	~ObjectPool();

	void FeedObject(Entity::Ptr _obj);
	void Clear();

	Entity* BorrowObject();
};
