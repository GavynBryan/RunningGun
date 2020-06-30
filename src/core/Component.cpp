#include <core/Component.h>



Component::Component(Entity& _entity)
	:mEntity(_entity),
	active(true)
{
}


Component::~Component()
{
}

void Component::start()
{

}

void Component::update()
{

}

void Component::postUpdate()
{

}

void Component::onCollide(Entity& _other)
{

}