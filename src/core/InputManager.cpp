#include <core/InputManager.h>

InputManager* InputManager::instance = nullptr;

InputManager::InputManager() {
    instance = this;
}

InputManager::~InputManager() {
    if (instance == this) {
        instance = nullptr;
    }
}

InputManager& InputManager::Instance() {
    return *instance;
}

void InputManager::beginFrame() {
    // Copy current state to previous state
    previousKeys = currentKeys;

    // Clear key states for the new frame
    for (auto& pair : keyStates) {
        if (pair.second == KeyState::Pressed) {
            pair.second = KeyState::Held;
        } else if (pair.second == KeyState::Released) {
            pair.second = KeyState::None;
        }
    }
}

void InputManager::processEvent(const SDL_Event& event) {
    if (event.type == SDL_EVENT_KEY_DOWN) {
        SDL_Scancode key = event.key.scancode;
        if (!event.key.repeat) {
            currentKeys[key] = true;
            keyStates[key] = KeyState::Pressed;
        }
    } else if (event.type == SDL_EVENT_KEY_UP) {
        SDL_Scancode key = event.key.scancode;
        currentKeys[key] = false;
        keyStates[key] = KeyState::Released;
    }
}

void InputManager::endFrame() {
    // Update held states based on current key state
    for (auto& pair : currentKeys) {
        if (pair.second && keyStates[pair.first] != KeyState::Pressed) {
            keyStates[pair.first] = KeyState::Held;
        }
    }
}

bool InputManager::isKeyPressed(SDL_Scancode key) const {
    auto it = keyStates.find(key);
    if (it != keyStates.end()) {
        return it->second == KeyState::Pressed;
    }
    return false;
}

bool InputManager::isKeyHeld(SDL_Scancode key) const {
    auto it = currentKeys.find(key);
    if (it != currentKeys.end()) {
        return it->second;
    }
    return false;
}

bool InputManager::isKeyReleased(SDL_Scancode key) const {
    auto it = keyStates.find(key);
    if (it != keyStates.end()) {
        return it->second == KeyState::Released;
    }
    return false;
}

bool InputManager::isKeyDown(SDL_Scancode key) const {
    return isKeyHeld(key);
}

KeyState InputManager::getKeyState(SDL_Scancode key) const {
    auto it = keyStates.find(key);
    if (it != keyStates.end()) {
        return it->second;
    }
    return KeyState::None;
}
