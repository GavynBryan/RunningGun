#pragma once
#include <array>
#include <memory>
#include <vector>
#include <core/math/Rect.h>

struct QuadTreeProxy;

class QuadTree
{
public:
	QuadTree(const Rectf& bounds, int capacity = 6, int maxDepth = 6, int depth = 0);

	void Clear();
	void Insert(QuadTreeProxy* proxy);
	bool Remove(QuadTreeProxy* proxy);
	void Query(const Rectf& range, std::vector<QuadTreeProxy*>& found) const;

	// Update a proxy's position (remove + reinsert)
	void Update(QuadTreeProxy* proxy);

private:
	Rectf Bounds;
	int Capacity;
	int MaxDepth;
	int Depth;
	std::vector<QuadTreeProxy*> Items;
	std::array<std::unique_ptr<QuadTree>, 4> Children;

	bool InsertIntoChild(QuadTreeProxy* proxy);
	void Subdivide();
	bool ContainsRect(const Rectf& outer, const Rectf& inner) const;
};
