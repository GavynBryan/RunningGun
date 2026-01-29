#pragma once

#include <string>

class PlayerComponent;

class BasePlayerAction {
public:
	virtual ~BasePlayerAction() = default;
	virtual void Execute(PlayerComponent& player) = 0;

	virtual float GetCooldownSeconds() const = 0;
	virtual std::string GetName() const = 0;
};