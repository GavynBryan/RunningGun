#pragma once
#include <memory>
#include <core/base/ActorComponent.h>
#include <core/base/ComponentMacros.h>
#include <core/math/Vec2.h>
#include <core/events/MulticastDelegate.h>

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
class PlayerInputConfig;

class PlayerComponent :
	public ActorComponent
{
	COMPONENT(PlayerComponent, "player")

public:
	const char* GetName() const override { return "PlayerComponent"; }

private:
	TimeService& Time;
	InputService& Input;
	ObjectPoolService& ObjectPool;
	const PlayerInputConfig& InputConfig;

	AnimatorComponent*					Animator;
	RigidBody2DComponent*				PhysicsHandle;

	std::unique_ptr<PlayerAction>		MoveLeftActionHandle;
	std::unique_ptr<PlayerAction>		MoveRightActionHandle;
	std::unique_ptr<PlayerAction>		JumpActionHandle;
	std::unique_ptr<PlayerAction>		ShootActionHandle;

	Vec2								BulletOffset;
	Vec2								MovementIntent;

	// Serialized properties (via PROPERTY macros)
	int m_Lives;
	float m_PlayerSpeed;
	float m_GroundAcceleration;
	float m_GroundDeceleration;

	// Runtime state (not serialized)
	float								LastShotTime;
	bool								IsInvulnerable;
	float								InvulnerabilityEndTime;
	bool								IsInputEnabled;

public:
									PlayerComponent(Actor& _entity, GameServiceHost& _context);
									~PlayerComponent();


	void							Start();
	void							Update();

	void							HandleAnimations();

	void							ShootBullet();
	int								GetHealth() const { return m_Lives; }
	float							GetPlayerSpeed() const { return m_PlayerSpeed; }
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
