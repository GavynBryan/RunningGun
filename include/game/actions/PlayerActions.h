#pragma once

#include <core/BasePlayerAction.h>
#include <game/components/PlayerComponent.h>

class MoveLeftAction final : public BasePlayerAction {
public:
	void Execute(PlayerComponent& player) override {
		player.SetMovementIntentX(-1.0f);
	}

	float GetCooldownSeconds() const override {
		return 0.0f;
	}

	std::string GetName() const override {
		return "Move Left";
	}
};

class MoveRightAction final : public BasePlayerAction {
public:
	void Execute(PlayerComponent& player) override {
		player.SetMovementIntentX(1.0f);
	}

	float GetCooldownSeconds() const override {
		return 0.0f;
	}

	std::string GetName() const override {
		return "Move Right";
	}
};

class JumpAction final : public BasePlayerAction {
public:
	void Execute(PlayerComponent& player) override {
		if (player.IsGrounded()) {
			player.RequestJump(JumpSpeed);
		}
	}

	float GetCooldownSeconds() const override {
		return 0.0f;
	}

	std::string GetName() const override {
		return "Jump";
	}
};

class ShootAction final : public BasePlayerAction {
public:
	void Execute(PlayerComponent& player) override {
		player.ShootBullet();
	}

	float GetCooldownSeconds() const override {
		return 0.1f;
	}

	std::string GetName() const override {
		return "Shoot";
	}
};
