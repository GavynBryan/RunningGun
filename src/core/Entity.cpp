#include <iostream>
#include <cmath>
#include <core/Entity.h>
#include <core/Game.h>


Entity::Entity(std::string _texture, float _width, float _height)
	:mPosition(0,0),
	mVelocity(0,0),
	activated(true),
	markedForDeath(false)
{
	//load resource handler from service
	auto handler = sEnvironment::Instance().getTextureHandler();
	mSprite.setTexture(handler->get(_texture));
	mSprite.setTextureRect(Recti(0, 0, static_cast<int>(_width), static_cast<int>(_height)));
}


Entity::~Entity()
{
}

void Entity::attachComponent(std::unique_ptr<Component> _comp)
{
	mComponents.push_back(std::move(_comp));
}

void Entity::startComponents()
{
	if (activated) {
		for (auto& c : mComponents) {
			c->start();
		}
	}
}

void Entity::updateComponents()
{
	if (activated) {
		for (auto& c : mComponents) {
			c->update();
		}
	}
}

void Entity::postUpdateComponents()
{
	if (activated) {
		for (auto& c : mComponents) {
			c->postUpdate();
		}
	}
}

void Entity::start()
{
	startComponents();
	//so that collision isn't detected on the first frame
	mSprite.setPosition(mPosition);
}

void Entity::update()
{
	if (activated) {
		updateComponents();
		mPosition += (mVelocity * sEnvironment::Instance().deltaTime());
	}
}

void Entity::postUpdate()
{
	if (activated) {
		postUpdateComponents();

		if (mAnimator != nullptr) {
			mAnimator->update(mSprite);
		}
	}
}

void Entity::render(SDL_Renderer* renderer)
{
	if (activated) {
		mSprite.setPosition(mPosition);
		mSprite.render(renderer);
	}
}

void Entity::setPosition(float x, float y)
{
	mPosition.x = x; mPosition.y = y;
	mSprite.setPosition(mPosition);
}

void Entity::setPosition(Vec2 _pos)
{
	mPosition = _pos;
	mSprite.setPosition(mPosition);
}

void Entity::setVelocity(float x, float y)
{
	mVelocity.x = x; mVelocity.y = y;
}

void Entity::setVelocity(Vec2 _vel)
{
	mVelocity = _vel;
}

void Entity::setDirection(Vec2 _dir)
{
	mDirection = _dir;
}

void Entity::setDirection(float x, float y)
{
	mDirection.x = x; mDirection.y = y;
}

void Entity::assignAnimator(std::unique_ptr<AnimationListener> _animator)
{
	mAnimator = std::move(_animator);
}

bool Entity::isGrounded()
{
	return (std::abs(mPosition.y - sEnvironment::Instance().getPhysics()->getGroundLevel())) <= 0.3f;
}

void Entity::onCollide(Entity& _other)
{
	if (activated) {
		for (auto& c : mComponents) {
			c->onCollide(_other);
		}
	}
}
