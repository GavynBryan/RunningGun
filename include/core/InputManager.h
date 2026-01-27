#pragma once
#include <SDL3/SDL.h>
#include <unordered_map>

enum class KeyState {
    None,
    Pressed,   // Key was just pressed this frame
    Held,      // Key is being held down
    Released   // Key was just released this frame
};

class InputManager {
private:
    static InputManager* instance;

    std::unordered_map<SDL_Scancode, KeyState> keyStates;
    std::unordered_map<SDL_Scancode, bool> currentKeys;
    std::unordered_map<SDL_Scancode, bool> previousKeys;

public:
    InputManager();
    ~InputManager();

    static InputManager& Instance();

    // Call at the beginning of each frame before processing events
    void beginFrame();

    // Process an SDL event (call for each event in the event loop)
    void processEvent(const SDL_Event& event);

    // Call at the end of event processing to finalize states
    void endFrame();

    // Query key states
    bool isKeyPressed(SDL_Scancode key) const;   // True only on the frame key was pressed
    bool isKeyHeld(SDL_Scancode key) const;      // True while key is down (including press frame)
    bool isKeyReleased(SDL_Scancode key) const;  // True only on the frame key was released
    bool isKeyDown(SDL_Scancode key) const;      // Alias for isKeyHeld

    KeyState getKeyState(SDL_Scancode key) const;
};
