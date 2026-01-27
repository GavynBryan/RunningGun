#include <core/InputManager.h>

InputManager* InputManager::InstancePtr = nullptr;

InputManager::InputManager() {
    InstancePtr = this;
}

InputManager::~InputManager() {
    if (InstancePtr == this) {
        InstancePtr = nullptr;
    }
}

InputManager& InputManager::Instance() {
    return *InstancePtr;
}

void InputManager::BeginFrame() {
    // Copy current state to previous state
    PreviousKeys = CurrentKeys;

    // Clear key states for the new frame
    for (auto& _pair : KeyStates) {
        if (_pair.second == KeyState::Pressed) {
            _pair.second = KeyState::Held;
        } else if (_pair.second == KeyState::Released) {
            _pair.second = KeyState::None;
        }
    }
}

void InputManager::ProcessEvent(const SDL_Event& _event) {
    if (_event.type == SDL_EVENT_KEY_DOWN) {
        SDL_Scancode _key = _event.key.scancode;
        if (!_event.key.repeat) {
            CurrentKeys[_key] = true;
            KeyStates[_key] = KeyState::Pressed;
        }
    } else if (_event.type == SDL_EVENT_KEY_UP) {
        SDL_Scancode _key = _event.key.scancode;
        CurrentKeys[_key] = false;
        KeyStates[_key] = KeyState::Released;
    }
}

void InputManager::EndFrame() {
    // Update held states based on current key state
    for (auto& _pair : CurrentKeys) {
        if (_pair.second && KeyStates[_pair.first] != KeyState::Pressed) {
            KeyStates[_pair.first] = KeyState::Held;
        }
    }
}

bool InputManager::IsKeyPressed(SDL_Scancode _key) const {
    auto _found = KeyStates.find(_key);
    if (_found != KeyStates.end()) {
        return _found->second == KeyState::Pressed;
    }
    return false;
}

bool InputManager::IsKeyHeld(SDL_Scancode _key) const {
    auto _found = CurrentKeys.find(_key);
    if (_found != CurrentKeys.end()) {
        return _found->second;
    }
    return false;
}

bool InputManager::IsKeyReleased(SDL_Scancode _key) const {
    auto _found = KeyStates.find(_key);
    if (_found != KeyStates.end()) {
        return _found->second == KeyState::Released;
    }
    return false;
}

bool InputManager::IsKeyDown(SDL_Scancode _key) const {
    return IsKeyHeld(_key);
}

KeyState InputManager::GetKeyState(SDL_Scancode _key) const {
    auto _found = KeyStates.find(_key);
    if (_found != KeyStates.end()) {
        return _found->second;
    }
    return KeyState::None;
}
