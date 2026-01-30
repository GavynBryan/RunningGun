#include <core/Component.h>
#include <core/engine/GameServiceHost.h>



Component::Component(Entity& _entity, GameServiceHost& _context)
	:ParentEntity(_entity),
	Context(_context),
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
