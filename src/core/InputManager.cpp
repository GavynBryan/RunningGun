#include <core/InputManager.h>

InputManager::InputManager() {
	KeyStates.fill(ActionState::None);
	CurrentKeys.fill(false);
}

InputManager::~InputManager() {
}

void InputManager::BeginFrame() {
	for (auto& _state : KeyStates) {
		if (_state == ActionState::Pressed) {
			_state = ActionState::Held;
		} else if (_state == ActionState::Released) {
			_state = ActionState::None;
		}
	}
}

void InputManager::ProcessEvent(const SDL_Event& _event) {
	if (_event.type == SDL_EVENT_KEY_DOWN) {
		SDL_Scancode _key = _event.key.scancode;
		if (!_event.key.repeat && _key != SDL_SCANCODE_UNKNOWN) {
			CurrentKeys[_key] = true;
			KeyStates[_key] = ActionState::Pressed;
		}
	} else if (_event.type == SDL_EVENT_KEY_UP) {
		SDL_Scancode _key = _event.key.scancode;
		if (_key != SDL_SCANCODE_UNKNOWN) {
			CurrentKeys[_key] = false;
			KeyStates[_key] = ActionState::Released;
		}
	}
}

void InputManager::EndFrame() {
	for (size_t _index = 0; _index < CurrentKeys.size(); ++_index) {
		if (CurrentKeys[_index] && KeyStates[_index] != ActionState::Pressed) {
			KeyStates[_index] = ActionState::Held;
		}
	}
}

bool InputManager::IsKeyPressed(SDL_Scancode _key) const {
	return KeyStates[_key] == ActionState::Pressed;
}

bool InputManager::IsKeyHeld(SDL_Scancode _key) const {
	return CurrentKeys[_key];
}

bool InputManager::IsKeyReleased(SDL_Scancode _key) const {
	return KeyStates[_key] == ActionState::Released;
}

bool InputManager::IsKeyDown(SDL_Scancode _key) const {
	return IsKeyHeld(_key);
}

ActionState InputManager::GetKeyState(SDL_Scancode _key) const {
	return KeyStates[_key];
}
