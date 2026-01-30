#pragma once

#include <SDL3/SDL.h>
#include <array>
#include <string>
#include <unordered_map>
#include <game/input/InputActions.h>

class PlayerInputConfig
{
public:
	PlayerInputConfig();

	void ApplyBindings(const std::unordered_map<std::string, SDL_Scancode>& _bindings);
	SDL_Scancode GetBinding(InputAction _action) const;

private:
	std::array<SDL_Scancode, static_cast<size_t>(InputAction::Count)> ActionBindings;

	size_t GetActionIndex(InputAction _action) const;
};
