#include <core/InputManager.h>
#include <core/Json.h>

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
	auto _result = Json::ParseFile(_path);
	if (_result.error()) {
		return false;
	}

	simdjson::dom::element _root = _result.value();
	auto _bindings = _root["bindings"].get_object();
	if (_bindings.error()) {
		return false;
	}

	for (auto _field : _bindings.value()) {
		auto _keyName = _field.value.get_string();
		if (_keyName.error()) {
			continue;
		}

		InputAction _action;
		if (!TryParseActionName(_field.key, _action)) {
			continue;
		}

		std::string _keyStr(_keyName.value());
		SDL_Scancode _scancode = SDL_GetScancodeFromName(_keyStr.c_str());
		if (_scancode == SDL_SCANCODE_UNKNOWN) {
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
