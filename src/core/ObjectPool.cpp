#include <core/ObjectPool.h>
#include <core/engine/GameplayServices.h>



ObjectPool::ObjectPool(GameplayServices& _context)
	:Context(_context)
{
}


ObjectPool::~ObjectPool()
{
}

void ObjectPool::FeedObject(Entity::Ptr _obj)
{
	_obj->Disable();
	Pool.push_back(_obj.get());
	Context.Instantiate(std::move(_obj));
}

void ObjectPool::Clear()
{
	Pool.clear();
}


Entity* ObjectPool::BorrowObject() 
{
	for (auto& _entity : Pool) {
		if (!_entity->IsEnabled()) {
			_entity->Enable();
			return _entity;
		}
	}
	return nullptr;
}
