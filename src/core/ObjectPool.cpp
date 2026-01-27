#include <core/ObjectPool.h>



ObjectPool::ObjectPool()
{
}


ObjectPool::~ObjectPool()
{
}

void ObjectPool::FeedObject(Entity::Ptr _obj)
{
	_obj->Disable();
	Pool.push_back(_obj.get());
	Environment::Instance().Instantiate(std::move(_obj));
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
