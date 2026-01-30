#pragma once
#include <core/Entity.h>

class GameplayServices;

class ObjectPool
{
private:
	GameplayServices& Context;
	std::vector<Entity*> Pool;
public:
	ObjectPool(GameplayServices& _context);
	~ObjectPool();

	void FeedObject(Entity::Ptr _obj);
	void Clear();

	Entity* BorrowObject();
};
