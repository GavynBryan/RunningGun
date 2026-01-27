#include <core/QuadTree.h>

QuadTree::QuadTree(const Rectf& _bounds, int _capacity, int _maxDepth, int _depth)
	: Bounds(_bounds),
	Capacity(_capacity),
	MaxDepth(_maxDepth),
	Depth(_depth)
{
}

void QuadTree::Clear()
{
	Items.clear();
	for (auto& _child : Children) {
		_child.reset();
	}
}

void QuadTree::Insert(Entity* _entity)
{
	const Rectf _bounds = _entity->GetBoundingRect();
	if (!Bounds.Intersects(_bounds)) {
		return;
	}

	if (Children[0] && InsertIntoChild(_entity)) {
		return;
	}

	Items.push_back(_entity);

	if (Items.size() > static_cast<size_t>(Capacity) && Depth < MaxDepth) {
		if (!Children[0]) {
			Subdivide();
		}
		auto _iter = Items.begin();
		while (_iter != Items.end()) {
			if (InsertIntoChild(*_iter)) {
				_iter = Items.erase(_iter);
			} else {
				++_iter;
			}
		}
	}
}

void QuadTree::Query(const Rectf& _range, std::vector<Entity*>& _found) const
{
	if (!Bounds.Intersects(_range)) {
		return;
	}

	for (auto* _entity : Items) {
		if (_entity->GetBoundingRect().Intersects(_range)) {
			_found.push_back(_entity);
		}
	}

	if (Children[0]) {
		for (const auto& _child : Children) {
			_child->Query(_range, _found);
		}
	}
}

bool QuadTree::InsertIntoChild(Entity* _entity)
{
	const Rectf _bounds = _entity->GetBoundingRect();
	for (auto& _child : Children) {
		if (_child && ContainsRect(_child->Bounds, _bounds)) {
			_child->Insert(_entity);
			return true;
		}
	}
	return false;
}

void QuadTree::Subdivide()
{
	const float _halfWidth = Bounds.width / 2.0f;
	const float _halfHeight = Bounds.height / 2.0f;
	const float _x = Bounds.x;
	const float _y = Bounds.y;

	Children[0] = std::make_unique<QuadTree>(Rectf(_x, _y, _halfWidth, _halfHeight), Capacity, MaxDepth, Depth + 1);
	Children[1] = std::make_unique<QuadTree>(Rectf(_x + _halfWidth, _y, _halfWidth, _halfHeight), Capacity, MaxDepth, Depth + 1);
	Children[2] = std::make_unique<QuadTree>(Rectf(_x, _y + _halfHeight, _halfWidth, _halfHeight), Capacity, MaxDepth, Depth + 1);
	Children[3] = std::make_unique<QuadTree>(Rectf(_x + _halfWidth, _y + _halfHeight, _halfWidth, _halfHeight), Capacity, MaxDepth, Depth + 1);
}

bool QuadTree::ContainsRect(const Rectf& _outer, const Rectf& _inner) const
{
	return _inner.Left() >= _outer.Left()
		&& _inner.Right() <= _outer.Right()
		&& _inner.Top() >= _outer.Top()
		&& _inner.Bottom() <= _outer.Bottom();
}
