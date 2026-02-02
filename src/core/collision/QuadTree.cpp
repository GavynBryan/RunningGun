#include <collision/QuadTree.h>
#include <collision/QuadTreeProxy.h>
#include <algorithm>

QuadTree::QuadTree(const Rectf& bounds, int capacity, int maxDepth, int depth)
	: Bounds(bounds)
	, Capacity(capacity)
	, MaxDepth(maxDepth)
	, Depth(depth)
{
	Items.reserve(capacity);
}

void QuadTree::Clear()
{
	Items.clear();
	for (auto& child : Children) {
		child.reset();
	}
}

void QuadTree::Insert(QuadTreeProxy* proxy)
{
	if (!proxy || !proxy->Enabled) {
		return;
	}

	// If we have children, try to insert into them first
	if (Children[0]) {
		if (InsertIntoChild(proxy)) {
			return;
		}
	}

	// Add to this node
	Items.push_back(proxy);

	// Check if we need to subdivide
	if (Children[0] == nullptr && 
	    static_cast<int>(Items.size()) > Capacity && 
	    Depth < MaxDepth) {
		Subdivide();

		// Try to move items to children
		auto it = Items.begin();
		while (it != Items.end()) {
			if (InsertIntoChild(*it)) {
				it = Items.erase(it);
			} else {
				++it;
			}
		}
	}
}

bool QuadTree::Remove(QuadTreeProxy* proxy)
{
	if (!proxy) {
		return false;
	}

	// Try to remove from this node
	auto it = std::find(Items.begin(), Items.end(), proxy);
	if (it != Items.end()) {
		Items.erase(it);
		return true;
	}

	// Try to remove from children
	if (Children[0]) {
		for (auto& child : Children) {
			if (child->Remove(proxy)) {
				return true;
			}
		}
	}

	return false;
}

void QuadTree::Query(const Rectf& range, std::vector<QuadTreeProxy*>& found) const
{
	// Check if range intersects this node's bounds
	if (!Bounds.Intersects(range)) {
		return;
	}

	// Check items in this node
	for (auto* proxy : Items) {
		if (proxy->Enabled && range.Intersects(proxy->Bounds)) {
			found.push_back(proxy);
		}
	}

	// Query children
	if (Children[0]) {
		for (const auto& child : Children) {
			child->Query(range, found);
		}
	}
}

void QuadTree::Update(QuadTreeProxy* proxy)
{
	if (!proxy) {
		return;
	}

	// Simple approach: remove and reinsert
	Remove(proxy);
	if (proxy->Enabled) {
		Insert(proxy);
	}
}

bool QuadTree::InsertIntoChild(QuadTreeProxy* proxy)
{
	if (!Children[0]) {
		return false;
	}

	for (auto& child : Children) {
		if (ContainsRect(child->Bounds, proxy->Bounds)) {
			child->Insert(proxy);
			return true;
		}
	}

	return false;
}

void QuadTree::Subdivide()
{
	float halfWidth = Bounds.width / 2.0f;
	float halfHeight = Bounds.height / 2.0f;
	float x = Bounds.x;
	float y = Bounds.y;

	// Top-left
	Children[0] = std::make_unique<QuadTree>(
		Rectf(x, y, halfWidth, halfHeight),
		Capacity, MaxDepth, Depth + 1
	);

	// Top-right
	Children[1] = std::make_unique<QuadTree>(
		Rectf(x + halfWidth, y, halfWidth, halfHeight),
		Capacity, MaxDepth, Depth + 1
	);

	// Bottom-left
	Children[2] = std::make_unique<QuadTree>(
		Rectf(x, y + halfHeight, halfWidth, halfHeight),
		Capacity, MaxDepth, Depth + 1
	);

	// Bottom-right
	Children[3] = std::make_unique<QuadTree>(
		Rectf(x + halfWidth, y + halfHeight, halfWidth, halfHeight),
		Capacity, MaxDepth, Depth + 1
	);
}

bool QuadTree::ContainsRect(const Rectf& outer, const Rectf& inner) const
{
	return inner.Left() >= outer.Left() &&
	       inner.Right() <= outer.Right() &&
	       inner.Top() >= outer.Top() &&
	       inner.Bottom() <= outer.Bottom();
}
