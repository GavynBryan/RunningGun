#pragma once
class Entity;
class GameplayServices;

class Component
{
protected:
	Entity&	ParentEntity;
	GameplayServices&	Context;
	bool	Active;
public:
	Component(Entity& _entity, GameplayServices& _context);
	~Component();

	GameplayServices& GetContext() { return Context; }

	virtual void	Start();
	virtual void	Update();
	virtual void	PostUpdate();
	virtual void	OnCollide(Entity& _other);
};
