#include <iostream>
#include <core/Entity.h>
#include <core/Game.h>


Entity::Entity(std::string _texture, float _width, float _height)
	:mPosition(0,0),
	mVelocity(0,0),
	activated(true)
{
	//load resource handler from service
	auto &handler = *sEnvironment::Instance().getTextureHandler();
	mSprite.setTexture(handler.get(_texture));
	mSprite.setTextureRect(sf::IntRect(0, 0, _width, _height));
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

void Entity::render(sf::RenderTarget& _target)
{
	if (activated) {
		mSprite.setPosition(mPosition);
		_target.draw(mSprite);
	}
}
/*
When setPosition is called, call it on the Sprite as well,
to avoid collision box issues in the game loop.
*/
void Entity::setPosition(float x, float y)
{
	mPosition.x = x; mPosition.y = y;
	mSprite.setPosition(mPosition);
}

void Entity::setPosition(sf::Vector2f _pos)
{
	mPosition = _pos;
	mSprite.setPosition(mPosition);
}

void Entity::setVelocity(float x, float y)
{
	mVelocity.x = x; mVelocity.y = y;
}

void Entity::setVelocity(sf::Vector2f _vel)
{
	mVelocity = _vel;
}

void Entity::setDirection(sf::Vector2f _dir) 
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
	return (abs(mPosition.y - sEnvironment::Instance().getPhysics()->getGroundLevel())) <= 0.3f;
}

void Entity::onCollide(Entity& _other)
{
	if (activated) {
		for (auto& c : mComponents) {
			c->onCollide(_other);
		}
	}
}