#pragma once
#include <SDL3/SDL.h>
#include <array>

enum class ActionState {
    None,
    Pressed,   // Key was just pressed this frame
    Held,      // Key is being held down
    Released   // Key was just released this frame
};

class InputManager {
private:
    std::array<ActionState, SDL_NUM_SCANCODES> KeyStates;
    std::array<bool, SDL_NUM_SCANCODES> CurrentKeys;

public:
    InputManager();
    ~InputManager();

    // Call at the beginning of each frame before processing events
    void BeginFrame();

    // Process an SDL event (call for each event in the event loop)
    void ProcessEvent(const SDL_Event& _event);

    // Call at the end of event processing to finalize states
    void EndFrame();

    // Query key states
    bool IsKeyPressed(SDL_Scancode _key) const;   // True only on the frame key was pressed
    bool IsKeyHeld(SDL_Scancode _key) const;      // True while key is down (including press frame)
    bool IsKeyReleased(SDL_Scancode _key) const;  // True only on the frame key was released
    bool IsKeyDown(SDL_Scancode _key) const;      // Alias for IsKeyHeld

    ActionState GetKeyState(SDL_Scancode _key) const;
};
