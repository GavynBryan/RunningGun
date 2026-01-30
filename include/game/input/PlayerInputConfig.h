#pragma once

#include <SDL3/SDL.h>
#include <array>
#include <string>
#include <unordered_map>
#include <game/input/InputActions.h>

class InputManager;

class PlayerInputConfig
{
public:
	PlayerInputConfig();

	// Load key bindings from a JSON file and apply them
	bool LoadBindings(const std::string& _path);

	// Apply bindings from a map (action name string -> scancode)
	void ApplyBindings(const std::unordered_map<std::string, SDL_Scancode>& _bindings);

	// Get the scancode bound to an action
	SDL_Scancode GetBinding(InputAction _action) const;

	// Convenience methods for querying action state
	bool IsActionPressed(InputAction _action, const InputManager& _input) const;
	bool IsActionHeld(InputAction _action, const InputManager& _input) const;
	bool IsActionReleased(InputAction _action, const InputManager& _input) const;

	// Register an action name mapping (call during static init or setup)
	static void RegisterAction(const std::string& _name, InputAction _action);

	// Parse action name to enum (uses registered mappings)
	static bool TryParseAction(const std::string& _name, InputAction& _outAction);

private:
	std::array<SDL_Scancode, static_cast<size_t>(InputAction::Count)> ActionBindings;

	size_t GetActionIndex(InputAction _action) const;

	// Registry of action name -> enum mappings
	static std::unordered_map<std::string, InputAction>& GetActionRegistry();
};
