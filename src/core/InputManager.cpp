#include <core/InputManager.h>
#include <core/JsonParser.h>

namespace {
	bool TryParseActionName(const std::string& _name, InputAction& _action) {
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

InputManager* InputManager::InstancePtr = nullptr;

InputManager::InputManager() {
	InstancePtr = this;
	ActionStates.fill(ActionState::None);
	CurrentActions.fill(false);
	ActionBindings.fill(SDL_SCANCODE_UNKNOWN);
	ActionBindings[GetActionIndex(InputAction::MoveLeft)] = SDL_SCANCODE_LEFT;
	ActionBindings[GetActionIndex(InputAction::MoveRight)] = SDL_SCANCODE_RIGHT;
	ActionBindings[GetActionIndex(InputAction::Jump)] = SDL_SCANCODE_Z;
	ActionBindings[GetActionIndex(InputAction::Shoot)] = SDL_SCANCODE_X;
}

InputManager::~InputManager() {
	if (InstancePtr == this) {
		InstancePtr = nullptr;
	}
}

InputManager& InputManager::Instance() {
	return *InstancePtr;
}

size_t InputManager::GetActionIndex(InputAction _action) const {
	return static_cast<size_t>(_action);
}

bool InputManager::TryGetAction(SDL_Scancode _key, InputAction& _action) const {
	for (size_t _index = 0; _index < ActionBindings.size(); ++_index) {
		if (ActionBindings[_index] == _key) {
			_action = static_cast<InputAction>(_index);
			return true;
		}
	}
	return false;
}

void InputManager::BeginFrame() {
	for (auto& _state : ActionStates) {
		if (_state == ActionState::Pressed) {
			_state = ActionState::Held;
		} else if (_state == ActionState::Released) {
			_state = ActionState::None;
		}
	}
}

void InputManager::ProcessEvent(const SDL_Event& _event) {
	InputAction _action;
	if (_event.type == SDL_EVENT_KEY_DOWN) {
		SDL_Scancode _key = _event.key.scancode;
		if (!_event.key.repeat && TryGetAction(_key, _action)) {
			auto _index = GetActionIndex(_action);
			CurrentActions[_index] = true;
			ActionStates[_index] = ActionState::Pressed;
		}
	} else if (_event.type == SDL_EVENT_KEY_UP) {
		SDL_Scancode _key = _event.key.scancode;
		if (TryGetAction(_key, _action)) {
			auto _index = GetActionIndex(_action);
			CurrentActions[_index] = false;
			ActionStates[_index] = ActionState::Released;
		}
	}
}

void InputManager::EndFrame() {
	for (size_t _index = 0; _index < CurrentActions.size(); ++_index) {
		if (CurrentActions[_index] && ActionStates[_index] != ActionState::Pressed) {
			ActionStates[_index] = ActionState::Held;
		}
	}
}

void InputManager::BindAction(InputAction _action, SDL_Scancode _key) {
	ActionBindings[GetActionIndex(_action)] = _key;
}

bool InputManager::LoadBindings(const std::string& _path) {
	JsonParseResult _result = JsonParser::ParseFile(_path);
	if (!_result) {
		SDL_Log("InputManager: Failed to parse %s: %s", _path.c_str(), _result.Error.c_str());
		return false;
	}

	const JsonValue& _root = _result.Root;
	if (!_root.IsObject()) {
		SDL_Log("InputManager: Root must be an object in %s", _path.c_str());
		return false;
	}

	const JsonValue& _bindings = _root["bindings"];
	if (!_bindings.IsObject()) {
		SDL_Log("InputManager: Missing bindings object in %s", _path.c_str());
		return false;
	}

	for (const auto& [_actionName, _keyValue] : _bindings.GetObject()) {
		if (!_keyValue.IsString()) {
			continue;
		}

		InputAction _action;
		if (!TryParseActionName(_actionName, _action)) {
			SDL_Log("InputManager: Unknown action name %s", _actionName.c_str());
			continue;
		}

		SDL_Scancode _scancode = SDL_GetScancodeFromName(_keyValue.GetString().c_str());
		if (_scancode == SDL_SCANCODE_UNKNOWN) {
			SDL_Log("InputManager: Unknown key binding %s for action %s",
				_keyValue.GetString().c_str(), _actionName.c_str());
			continue;
		}
		BindAction(_action, _scancode);
	}
	return true;
}

bool InputManager::IsActionPressed(InputAction _action) const {
	return ActionStates[GetActionIndex(_action)] == ActionState::Pressed;
}

bool InputManager::IsActionHeld(InputAction _action) const {
	return CurrentActions[GetActionIndex(_action)];
}

bool InputManager::IsActionReleased(InputAction _action) const {
	return ActionStates[GetActionIndex(_action)] == ActionState::Released;
}

bool InputManager::IsActionDown(InputAction _action) const {
	return IsActionHeld(_action);
}

ActionState InputManager::GetActionState(InputAction _action) const {
	return ActionStates[GetActionIndex(_action)];
}
