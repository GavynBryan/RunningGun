#pragma once
#include <memory>
#include <string_view>
#include <core/Component.h>
#include <core/PropertyDescriptor.h>
#include <core/Vec2.h>
#include <core/events/MulticastDelegate.h>

constexpr auto BulletCoolDown = .3f;
constexpr auto InvulnerabilityDuration = 1.0f;
constexpr auto DeathResetDelay = 3.0f;
constexpr auto JumpSpeed = 500.0f;

class PlayerAction;
class PhysicsComponent;
class PlayerComponent :
	public Component
{
public:
	// ========== Serialization ==========
	static constexpr const char* TypeName = "player";
	static constexpr PropertyDescriptor Properties[] = {
		{"groundAcceleration", "float", "2000.0", "Acceleration when speeding up"},
		{"groundDeceleration", "float", "3500.0", "Deceleration when slowing down"},
	};
	static std::unique_ptr<Component> Create(Entity& entity, GameServiceHost& context, std::string_view paramsJson);

private:
	uint8_t Lives;
	float PlayerSpeed;
	float GroundAcceleration;
	float GroundDeceleration;

	AnimationStateMachine*				Animator;
	PhysicsComponent*					PhysicsHandle;

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

public:
									PlayerComponent(Entity& _entity, GameServiceHost& _context);
									~PlayerComponent();


	void							Start();
	void							Update();
	void							PostUpdate();

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

	void							OnCollide(Entity& _other);

	// Events - GameMode subscribes to these
	MulticastDelegate<Entity*>		OnDied;
};
