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
	std::unique_ptr<AnimationStateMachine> _anim(new AnimationStateMachine());

	//set up animations
	AnimPtr _idle(new Animation(0, Vec2(64, 64), 1, true));
	AnimPtr _walk(new Animation(6, Vec2(64, 64), 1, true));
	AnimPtr _damage(new Animation(3, Vec2(64, 64), 1, false, true));
	AnimPtr _shoot(new Animation(7, Vec2(64, 64), 0, false));
	_anim->AddAnimation("idle", std::move(_idle));
	_anim->AddAnimation("walk", std::move(_walk));
	_anim->AddAnimation("shoot", std::move(_shoot));
	_anim->AddAnimation("damage", std::move(_damage));

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
	std::unique_ptr<AnimationStateMachine> _anim(new AnimationStateMachine());

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
	std::unique_ptr<AnimationStateMachine> _anim(new AnimationStateMachine());

	//setup animations
	AnimPtr _idle(new Animation(1, Vec2(64, 64), 1, true));
	AnimPtr _damage(new Animation(3, Vec2(64, 64), 0, false));
	_anim->AddAnimation("idle", std::move(_idle));
	_anim->AddAnimation("damage", std::move(_damage));

	_scorpion->AssignAnimator(std::move(_anim));

	_scorpion->SetTag(hazard);

	_scorpion->AttachComponent(std::move(_pc));
	_scorpion->AttachComponent(std::move(_phys));

	return _scorpion;
}
