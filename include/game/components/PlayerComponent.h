#pragma once
#include <memory>
#include <core/entity/Component.h>
#include <core/math/Vec2.h>
#include <core/events/MulticastDelegate.h>
#include <game/input/PlayerInputConfig.h>

constexpr auto BulletCoolDown = .3f;
constexpr auto InvulnerabilityDuration = 1.0f;
constexpr auto DeathResetDelay = 3.0f;
constexpr auto JumpSpeed = 500.0f;

class PlayerAction;
class RigidBody2DComponent;
class TimeService;
class InputService;
class ObjectPoolService;
class AnimatorComponent;

class PlayerComponent :
	public ActorComponent
{
public:
	const char* GetName() const override { return "PlayerComponent"; }

private:
	TimeService& Time;
	InputService& Input;
	ObjectPoolService& ObjectPool;

	uint8_t Lives;
	float PlayerSpeed;
	float GroundAcceleration;
	float GroundDeceleration;

	AnimatorComponent*					Animator;
	RigidBody2DComponent*						PhysicsHandle;

	Vec2							BulletOffset;
	float							LastShotTime;
	Vec2							MovementIntent;

	// Event-based flags and timers
	bool							IsInvulnerable;
	float							InvulnerabilityEndTime;
	bool							IsInputEnabled;

	std::unique_ptr<PlayerAction>	MoveLeftActionHandle;
	std::unique_ptr<PlayerAction>	MoveRightActionHandle;
	std::unique_ptr<PlayerAction>	JumpActionHandle;
	std::unique_ptr<PlayerAction>	ShootActionHandle;
	const PlayerInputConfig&		InputConfig;

public:
									PlayerComponent(Actor& _entity, GameServiceHost& _context, const PlayerInputConfig& _inputConfig);
									~PlayerComponent();


	void							Start();
	void							Update();

	void							HandleAnimations();

	void							ShootBullet();
	int								GetHealth() const { return Lives; }
	float							GetPlayerSpeed() const { return PlayerSpeed; }
	void							SetGroundAcceleration(float acceleration) { GroundAcceleration = acceleration; }
	void							SetGroundDeceleration(float deceleration) { GroundDeceleration = deceleration; }
	bool							IsGrounded() const;
	void							SetMovementIntentX(float x);
	void							RequestJump(float jumpSpeed);
	void							ApplyMovementIntent();
	void							SetHorizontalVelocity(float x);
	void							SetVerticalVelocity(float y);

	void							OrientDirection();
	void							HandleInput();
	void							Freeze();

	// Event handlers
	void							OnDamage();
	void							OnDeath();
	void							OnVictory();

	void							OnCollide(Actor& _other);

	// Events - GameMode subscribes to these
	MulticastDelegate<Actor*>		OnDied;
};
