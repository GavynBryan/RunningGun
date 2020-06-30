#include <core/ObjectPool.h>



ObjectPool::ObjectPool()
{
}


ObjectPool::~ObjectPool()
{
}

void ObjectPool::feedObject(Entity::Ptr _obj)
{
	_obj->disable();
	mPool.push_back(_obj.get());
	sEnvironment::Instance().Instantiate(std::move(_obj));
}

void ObjectPool::clear()
{
	mPool.clear();
}


Entity* ObjectPool::borrowObject() 
{
	for (auto& e : mPool) {
		if (!e->isEnabled()) {
			e->enable();
			return e;
		}
	}
	return nullptr;
}