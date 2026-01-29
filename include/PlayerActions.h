#pragma once

class PlayerComponent;

class PlayerAction {
public:
	virtual ~PlayerAction() = default;
	virtual void Execute(PlayerComponent& player) = 0;
};

class MoveLeftAction final : public PlayerAction {
public:
	void Execute(PlayerComponent& player) override;
};

class MoveRightAction final : public PlayerAction {
public:
	void Execute(PlayerComponent& player) override;
};

class JumpAction final : public PlayerAction {
public:
	void Execute(PlayerComponent& player) override;
};

class ShootAction final : public PlayerAction {
public:
	void Execute(PlayerComponent& player) override;
};
