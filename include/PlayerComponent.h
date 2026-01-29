#pragma once
#include <core/ObjectPool.h>
#include <core/Component.h>
#include <core/Vec2.h>

constexpr auto BulletCoolDown = .3f;
constexpr auto InvulnerabilityDuration = 1.0f;


class PlayerComponent :
	public Component
{
private:
	uint8_t Lives;
	float PlayerSpeed;

	AnimationListener*				Animator;
	ObjectPool						Bullets;

	Vec2							BulletOffset;
	float							LastShotTime;

	// Event-based flags and timers
	bool							IsInvulnerable;
	float							InvulnerabilityEndTime;
	bool							IsDead;
	bool							IsVictorious;

public:
									PlayerComponent(Entity& _entity);
									~PlayerComponent();


	void							Start();
	void							Update();
	void							PostUpdate();

	void							HandleAnimations();

	void							SetupBullets();
	void							ShootBullet();
	int								GetHealth() { return Lives; }

	void							OrientDirection();
	void							HandleInput();
	void							Freeze();

	// Event handlers
	void							OnDamage();
	void							OnDeath();
	void							OnVictory();

	void							OnCollide(Entity& _other);
};
