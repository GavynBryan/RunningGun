#pragma once
class Entity;

class Component
{
protected:
	Entity&	mEntity;
	bool		active;
public:
	Component(Entity& _entity);
	~Component();

	virtual void	start();
	virtual void	update();
	virtual void	postUpdate();
	virtual void	onCollide(Entity& _other);
};

