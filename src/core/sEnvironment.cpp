#include <core/sEnvironment.h>

sEnvironment *sEnvironment::instance = 0;

sEnvironment::sEnvironment()
{
	instance = this;
}


sEnvironment::~sEnvironment()
{
}

void sEnvironment::setTextureHandler(std::unique_ptr<ResourceHandler<sf::Texture>> _handler)
{
	TextureHandler = std::move(_handler);
}

void sEnvironment::setPhysics(std::unique_ptr<sPhysics> _physics)
{
	PhysicsManager = std::move(_physics);
}

void sEnvironment::setWorld(World* _world)
{
	world = _world;
}

void sEnvironment::setDeltaTime(float _dt)
{
	mDeltaTime = _dt;
}

ResourceHandler<sf::Texture>* sEnvironment::getTextureHandler() const
{
	assert(TextureHandler);
	return TextureHandler.get();
}

sPhysics* sEnvironment::getPhysics() const
{
	assert(PhysicsManager);
	return PhysicsManager.get();
}

float sEnvironment::getElapsedTime() const
{
	assert(world);
	return world->getElapsedTime();
}

float sEnvironment::deltaTime() const
{
	return mDeltaTime;
}

void sEnvironment::Instantiate(std::unique_ptr<Entity> _entity) const
{
	assert(world);
	world->addObject(std::move(_entity));
}

const sEnvironment& sEnvironment::Instance()
{
	assert(instance);
	return *instance;
}

void sEnvironment::Win() const
{
	assert(world);
	world->winGame();
}

void sEnvironment::Reset() const
{
	assert(world);
	world->reset();
}