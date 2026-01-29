#include <core/Prefabs.h>
#include <core/GameContext.h>
#include <PlayerComponent.h>
#include <PhysicsComponent.h>
#include <PatrolAIComponent.h>
#include <BullComponent.h>


Prefabs::Prefabs()
{
}


Prefabs::~Prefabs()
{
}

std::unique_ptr<Entity> Prefabs::GetPlayer(GameContext& _context)
{
	std::unique_ptr<Entity> _player(new Entity(_context, "sprites/player.png", 64, 64));

	std::unique_ptr<PhysicsComponent> _phys(new PhysicsComponent(*_player, _context));
	std::unique_ptr<PlayerComponent> _pc(new PlayerComponent(*_player, _context));
	std::unique_ptr<AnimationListener> _anim(new AnimationListener());

	//set up animations
	AnimPtr _right(new Animation(0, Vec2(64, 64), 1, true));
	AnimPtr _left(new Animation(1, Vec2(64, 64), 1, true));
	AnimPtr _walkRight(new Animation(6, Vec2(64, 64), 1, true));
	AnimPtr _walkLeft(new Animation(5, Vec2(64, 64), 1, true));
	AnimPtr _damageRight(new Animation(3, Vec2(64, 64), 1, false, true));
	AnimPtr _damageLeft(new Animation(4, Vec2(64, 64), 1, false, true));
	AnimPtr _shootRight(new Animation(7, Vec2(64, 64), 0, false));
	AnimPtr _shootLeft(new Animation(8, Vec2(64, 64), 0, false));
	_anim->AddAnimation("right", std::move(_right));
	_anim->AddAnimation("left", std::move(_left));
	_anim->AddAnimation("walkright", std::move(_walkRight));
	_anim->AddAnimation("walkleft", std::move(_walkLeft));
	_anim->AddAnimation("shootleft", std::move(_shootLeft));
	_anim->AddAnimation("shootright", std::move(_shootRight));
	_anim->AddAnimation("damageleft", std::move(_damageLeft));
	_anim->AddAnimation("damageright", std::move(_damageRight));

	_player->AssignAnimator(std::move(_anim));

	_player->AttachComponent(std::move(_pc));
	_player->AttachComponent(std::move(_phys));
	_player->SetPosition(0, 450);

	return _player;
}

std::unique_ptr<Entity> Prefabs::GetBull(GameContext& _context)
{
	std::unique_ptr<Entity> _bull(new Entity(_context, "sprites/bull.png", 256, 128));

	std::unique_ptr<BullComponent> _bc(new BullComponent(*_bull, _context));
	std::unique_ptr<AnimationListener> _anim(new AnimationListener());

	//setup animations
	AnimPtr _default(new Animation(0, Vec2(256, 128), 0, true));
	AnimPtr _shoot(new Animation(1, Vec2(256, 128), 0, false, true));
	AnimPtr _damage(new Animation(2, Vec2(256, 128), 0, false, true));
	AnimPtr _die(new Animation(3, Vec2(256, 128), 2, false, true));
	_anim->AddAnimation("default", std::move(_default));
	_anim->AddAnimation("shoot", std::move(_shoot));
	_anim->AddAnimation("damage", std::move(_damage));
	_anim->AddAnimation("die", std::move(_die));

	_bull->SetTag(hazard);
	_bull->AssignAnimator(std::move(_anim));

	_bull->AttachComponent(std::move(_bc));
	_bull->SetPosition(580, 415);

	return _bull;
}

std::unique_ptr<Entity> Prefabs::GetScorpion(GameContext& _context)
{
	std::unique_ptr<Entity> _scorpion(new Entity(_context, "sprites/scorpion.png", 64, 64));

	std::unique_ptr<PatrolAIComponent> _pc(new PatrolAIComponent(*_scorpion, _context, 150));
	std::unique_ptr<PhysicsComponent> _phys(new PhysicsComponent(*_scorpion, _context));
	std::unique_ptr<AnimationListener> _anim(new AnimationListener());

	//setup animations
	AnimPtr _left(new Animation(0, Vec2(64, 64), 1, true));
	AnimPtr _right(new Animation(1, Vec2(64, 64), 1, true));
	AnimPtr _damageLeft(new Animation(2, Vec2(64, 64), 0, false));
	AnimPtr _damageRight(new Animation(3, Vec2(64, 64), 0, false));
	_anim->AddAnimation("left", std::move(_left));
	_anim->AddAnimation("right", std::move(_right));
	_anim->AddAnimation("damageleft", std::move(_damageLeft));
	_anim->AddAnimation("damageright", std::move(_damageRight));

	_scorpion->AssignAnimator(std::move(_anim));

	_scorpion->SetTag(hazard);

	_scorpion->AttachComponent(std::move(_pc));
	_scorpion->AttachComponent(std::move(_phys));

	return _scorpion;
}
