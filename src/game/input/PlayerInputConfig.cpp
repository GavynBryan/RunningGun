#include <game/input/PlayerInputConfig.h>
#include <string_view>

namespace {
	bool TryParseActionName(std::string_view _name, InputAction& _action) {
		if (_name == "MoveLeft") {
			_action = InputAction::MoveLeft;
			return true;
		}
		if (_name == "MoveRight") {
			_action = InputAction::MoveRight;
			return true;
		}
		if (_name == "Jump") {
			_action = InputAction::Jump;
			return true;
		}
		if (_name == "Shoot") {
			_action = InputAction::Shoot;
			return true;
		}
		return false;
	}
}

PlayerInputConfig::PlayerInputConfig()
{
	ActionBindings.fill(SDL_SCANCODE_UNKNOWN);
	ActionBindings[GetActionIndex(InputAction::MoveLeft)] = SDL_SCANCODE_LEFT;
	ActionBindings[GetActionIndex(InputAction::MoveRight)] = SDL_SCANCODE_RIGHT;
	ActionBindings[GetActionIndex(InputAction::Jump)] = SDL_SCANCODE_Z;
	ActionBindings[GetActionIndex(InputAction::Shoot)] = SDL_SCANCODE_X;
}

void PlayerInputConfig::ApplyBindings(const std::unordered_map<std::string, SDL_Scancode>& _bindings)
{
	for (const auto& _pair : _bindings) {
		InputAction _action;
		if (!TryParseActionName(_pair.first, _action)) {
			continue;
		}
		ActionBindings[GetActionIndex(_action)] = _pair.second;
	}
}

SDL_Scancode PlayerInputConfig::GetBinding(InputAction _action) const
{
	return ActionBindings[GetActionIndex(_action)];
}

size_t PlayerInputConfig::GetActionIndex(InputAction _action) const
{
	return static_cast<size_t>(_action);
}
