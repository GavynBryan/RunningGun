#pragma once
class Entity;

class Component
{
protected:
	Entity&	ParentEntity;
	bool	Active;
public:
	Component(Entity& _entity);
	~Component();

	virtual void	Start();
	virtual void	Update();
	virtual void	PostUpdate();
	virtual void	OnCollide(Entity& _other);
};
