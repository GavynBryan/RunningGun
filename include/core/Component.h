#pragma once
class Entity;
class GameContext;

class Component
{
protected:
	Entity&	ParentEntity;
	GameContext&	Context;
	bool	Active;
public:
	Component(Entity& _entity, GameContext& _context);
	~Component();

	GameContext& GetContext() { return Context; }

	virtual void	Start();
	virtual void	Update();
	virtual void	PostUpdate();
	virtual void	OnCollide(Entity& _other);
};
