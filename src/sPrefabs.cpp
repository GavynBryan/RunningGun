#include <core/sPrefabs.h>
#include <PlayerComponent.h>
#include <PhysicsComponent.h>
#include <PatrolAIComponent.h>
#include <BullComponent.h>


sPrefabs::sPrefabs()
{
}


sPrefabs::~sPrefabs()
{
}

std::unique_ptr<Entity> sPrefabs::getPlayer() 
{
	std::unique_ptr<Entity> _player(new Entity("sprites/player.png", 64, 64));

	std::unique_ptr<PhysicsComponent> _phys(new PhysicsComponent(*_player));
	std::unique_ptr<PlayerComponent> _pc(new PlayerComponent(*_player));
	std::unique_ptr<AnimationListener> _anim(new AnimationListener());

	//set up animations
	AnimPtr _right(new Animation(0, sf::Vector2f(64, 64), 1, true));
	AnimPtr _left(new Animation(1, sf::Vector2f(64, 64), 1, true));
	AnimPtr _walkright(new Animation(6, sf::Vector2f(64, 64), 1, true));
	AnimPtr _walkleft(new Animation(5, sf::Vector2f(64, 64), 1, true));
	AnimPtr _damageright(new Animation(3, sf::Vector2f(64, 64), 1, false, true));
	AnimPtr _damageleft(new Animation(4, sf::Vector2f(64, 64), 1, false, true));
	AnimPtr _shootright(new Animation(7, sf::Vector2f(64, 64), 0, false));
	AnimPtr _shootleft(new Animation(8, sf::Vector2f(64, 64), 0, false));
	_anim->addAnimation("left", std::move(_left));
	_anim->addAnimation("right", std::move(_right));
	_anim->addAnimation("walkright", std::move(_walkright));
	_anim->addAnimation("walkleft", std::move(_walkleft));
	_anim->addAnimation("shootleft", std::move(_shootleft));
	_anim->addAnimation("shootright", std::move(_shootright));
	_anim->addAnimation("damageleft", std::move(_damageleft));
	_anim->addAnimation("damageright", std::move(_damageright));

	_player->assignAnimator(std::move(_anim));

	_player->attachComponent(std::move(_pc));
	_player->attachComponent(std::move(_phys));
	_player->setPosition(0, 450);

	return _player;
}

std::unique_ptr<Entity> sPrefabs::getBull() 
{
	std::unique_ptr<Entity> _bull(new Entity("sprites/bull.png", 256, 128));

	std::unique_ptr<BullComponent> _bc(new BullComponent(*_bull));
	std::unique_ptr<AnimationListener> _anim(new AnimationListener());

	//setup animations
	AnimPtr _default(new Animation(0, sf::Vector2f(256, 128), 0, true));
	AnimPtr _shoot(new Animation(1, sf::Vector2f(256, 128), 0, false, true));
	AnimPtr _damage(new Animation(2, sf::Vector2f(256, 128), 0, false, true));
	AnimPtr _die(new Animation(3, sf::Vector2f(256, 128), 2, false, true));
	_anim->addAnimation("default", std::move(_default));
	_anim->addAnimation("shoot", std::move(_shoot));
	_anim->addAnimation("damage", std::move(_damage));
	_anim->addAnimation("die", std::move(_die));
	
	_bull->setTag(hazard);
	_bull->assignAnimator(std::move(_anim));

	_bull->attachComponent(std::move(_bc));
	_bull->setPosition(580, 415);

	return _bull;
}

std::unique_ptr<Entity> sPrefabs::getScorpion() 
{
	std::unique_ptr<Entity> _scorpion(new Entity("sprites/scorpion.png", 64, 64));

	std::unique_ptr<PatrolAIComponent> _pc(new PatrolAIComponent(*_scorpion, 150));
	std::unique_ptr<PhysicsComponent> _phys(new PhysicsComponent(*_scorpion));
	std::unique_ptr<AnimationListener> _anim(new AnimationListener());

	//setup animations
	AnimPtr _left(new Animation(0, sf::Vector2f(64, 64), 1, true));
	AnimPtr _right(new Animation(1, sf::Vector2f(64, 64), 1, true));
	AnimPtr _damageleft(new Animation(2, sf::Vector2f(64, 64), 0, false));
	AnimPtr _damageright(new Animation(3, sf::Vector2f(64, 64), 0, false));
	_anim->addAnimation("left", std::move(_left));
	_anim->addAnimation("right", std::move(_right));
	_anim->addAnimation("damageleft", std::move(_damageleft));
	_anim->addAnimation("damageright", std::move(_damageright));

	_scorpion->assignAnimator(std::move(_anim));

	_scorpion->setTag(hazard);

	_scorpion->attachComponent(std::move(_pc));
	_scorpion->attachComponent(std::move(_phys));

	return _scorpion;
}