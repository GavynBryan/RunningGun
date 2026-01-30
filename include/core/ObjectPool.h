#pragma once
#include <core/Entity.h>

class GameServiceHost;

class ObjectPool
{
private:
	GameServiceHost& Context;
	std::vector<Entity*> Pool;
public:
	ObjectPool(GameServiceHost& _context);
	~ObjectPool();

	void FeedObject(Entity::Ptr _obj);
	void Clear();

	Entity* BorrowObject();
};
