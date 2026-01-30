#include <game/input/PlayerInputConfig.h>
#include <core/InputManager.h>
#include <core/Json.h>

namespace {
	// Auto-register default actions on first use
	struct ActionRegistrar {
		ActionRegistrar() {
			PlayerInputConfig::RegisterAction("MoveLeft", InputAction::MoveLeft);
			PlayerInputConfig::RegisterAction("MoveRight", InputAction::MoveRight);
			PlayerInputConfig::RegisterAction("Jump", InputAction::Jump);
			PlayerInputConfig::RegisterAction("Shoot", InputAction::Shoot);
		}
	};

	ActionRegistrar& EnsureActionsRegistered() {
		static ActionRegistrar registrar;
		return registrar;
	}
}

std::unordered_map<std::string, InputAction>& PlayerInputConfig::GetActionRegistry()
{
	static std::unordered_map<std::string, InputAction> registry;
	return registry;
}

void PlayerInputConfig::RegisterAction(const std::string& _name, InputAction _action)
{
	GetActionRegistry()[_name] = _action;
}

bool PlayerInputConfig::TryParseAction(const std::string& _name, InputAction& _outAction)
{
	EnsureActionsRegistered();
	auto& registry = GetActionRegistry();
	auto it = registry.find(_name);
	if (it != registry.end()) {
		_outAction = it->second;
		return true;
	}
	return false;
}

PlayerInputConfig::PlayerInputConfig()
{
	EnsureActionsRegistered();
	ActionBindings.fill(SDL_SCANCODE_UNKNOWN);
	ActionBindings[GetActionIndex(InputAction::MoveLeft)] = SDL_SCANCODE_LEFT;
	ActionBindings[GetActionIndex(InputAction::MoveRight)] = SDL_SCANCODE_RIGHT;
	ActionBindings[GetActionIndex(InputAction::Jump)] = SDL_SCANCODE_Z;
	ActionBindings[GetActionIndex(InputAction::Shoot)] = SDL_SCANCODE_X;
}

bool PlayerInputConfig::LoadBindings(const std::string& _path)
{
	auto _result = Json::ParseFile(_path);
	if (_result.error()) {
		return false;
	}

	simdjson::dom::element _root = _result.value();
	auto _bindingsObj = _root["bindings"].get_object();
	if (_bindingsObj.error()) {
		return false;
	}

	for (auto _field : _bindingsObj.value()) {
		auto _keyName = _field.value.get_string();
		if (_keyName.error()) {
			continue;
		}

		std::string _keyStr(_keyName.value());
		SDL_Scancode _scancode = SDL_GetScancodeFromName(_keyStr.c_str());
		if (_scancode == SDL_SCANCODE_UNKNOWN) {
			continue;
		}

		InputAction _action;
		if (TryParseAction(std::string(_field.key), _action)) {
			ActionBindings[GetActionIndex(_action)] = _scancode;
		}
	}

	return true;
}

void PlayerInputConfig::ApplyBindings(const std::unordered_map<std::string, SDL_Scancode>& _bindings)
{
	for (const auto& _pair : _bindings) {
		InputAction _action;
		if (!TryParseAction(_pair.first, _action)) {
			continue;
		}
		ActionBindings[GetActionIndex(_action)] = _pair.second;
	}
}

SDL_Scancode PlayerInputConfig::GetBinding(InputAction _action) const
{
	return ActionBindings[GetActionIndex(_action)];
}

bool PlayerInputConfig::IsActionPressed(InputAction _action, const InputManager& _input) const
{
	return _input.IsKeyPressed(GetBinding(_action));
}

bool PlayerInputConfig::IsActionHeld(InputAction _action, const InputManager& _input) const
{
	return _input.IsKeyHeld(GetBinding(_action));
}

bool PlayerInputConfig::IsActionReleased(InputAction _action, const InputManager& _input) const
{
	return _input.IsKeyReleased(GetBinding(_action));
}

size_t PlayerInputConfig::GetActionIndex(InputAction _action) const
{
	return static_cast<size_t>(_action);
}
