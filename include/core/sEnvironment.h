#pragma once
#include"ResourceHandler.h"
#include "sPhysics.h"
#include "World.h"

class sEnvironment
{
private:
	static sEnvironment*									instance;

	std::unique_ptr<ResourceHandler<sf::Texture>>			TextureHandler;
	std::unique_ptr<sPhysics>								PhysicsManager;
	World*													world;
	float													mDeltaTime;

public:
	void													setTextureHandler(std::unique_ptr<ResourceHandler<sf::Texture>> _handler);
	void													setPhysics(std::unique_ptr<sPhysics> _physics);
	void													setWorld(World* _world);
	void													setDeltaTime(float _dt);

	ResourceHandler<sf::Texture>*							getTextureHandler() const;
	sPhysics*												getPhysics() const;
	float													getElapsedTime() const;
	float													deltaTime() const;

	void													Instantiate(std::unique_ptr<Entity> _entity) const;

	void													Win() const;
	void													Reset() const;

public:
	sEnvironment();
	~sEnvironment();

	const static sEnvironment& Instance();
};

