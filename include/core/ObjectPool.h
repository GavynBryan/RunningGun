#pragma once
#include <core/Entity.h>

class GameContext;

class ObjectPool
{
private:
	GameContext& Context;
	std::vector<Entity*> Pool;
public:
	ObjectPool(GameContext& _context);
	~ObjectPool();

	void FeedObject(Entity::Ptr _obj);
	void Clear();

	Entity* BorrowObject();
};
