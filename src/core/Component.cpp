#include <core/Component.h>



Component::Component(Entity& _entity)
	:ParentEntity(_entity),
	Active(true)
{
}


Component::~Component()
{
}

void Component::Start()
{

}

void Component::Update()
{

}

void Component::PostUpdate()
{

}

void Component::OnCollide(Entity& _other)
{

}
