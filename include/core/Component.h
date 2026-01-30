#pragma once
class Entity;
class GameServiceHost;

class Component
{
protected:
	Entity&	ParentEntity;
	GameServiceHost&	Context;
	bool	Active;
public:
	Component(Entity& _entity, GameServiceHost& _context);
	~Component();

	GameServiceHost& GetContext() { return Context; }

	virtual void	Start();
	virtual void	Update();
	virtual void	PostUpdate();
	virtual void	OnCollide(Entity& _other);
};
