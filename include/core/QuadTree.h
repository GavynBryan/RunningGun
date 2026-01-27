#pragma once
#include <array>
#include <memory>
#include <vector>
#include "Rect.h"
#include "Entity.h"

class QuadTree
{
public:
	QuadTree(const Rectf& _bounds, int _capacity = 6, int _maxDepth = 6, int _depth = 0);

	void Clear();
	void Insert(Entity* _entity);
	void Query(const Rectf& _range, std::vector<Entity*>& _found) const;

private:
	Rectf Bounds;
	int Capacity;
	int MaxDepth;
	int Depth;
	std::vector<Entity*> Items;
	std::array<std::unique_ptr<QuadTree>, 4> Children;

	bool InsertIntoChild(Entity* _entity);
	void Subdivide();
	bool ContainsRect(const Rectf& _outer, const Rectf& _inner) const;
};
