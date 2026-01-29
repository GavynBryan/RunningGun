#pragma once
#include <SDL3/SDL.h>
#include <array>
#include <string>

enum class ActionState {
    None,
    Pressed,   // Key was just pressed this frame
    Held,      // Key is being held down
    Released   // Key was just released this frame
};

enum class InputAction {
    MoveLeft,
    MoveRight,
    Jump,
    Shoot,
    Count
};

class InputManager {
private:
    static InputManager* InstancePtr;

    std::array<ActionState, static_cast<size_t>(InputAction::Count)> ActionStates;
    std::array<bool, static_cast<size_t>(InputAction::Count)> CurrentActions;
    std::array<SDL_Scancode, static_cast<size_t>(InputAction::Count)> ActionBindings;

    bool TryGetAction(SDL_Scancode _key, InputAction& _action) const;
    size_t GetActionIndex(InputAction _action) const;

public:
    InputManager();
    ~InputManager();

    static InputManager& Instance();

    // Call at the beginning of each frame before processing events
    void BeginFrame();

    // Process an SDL event (call for each event in the event loop)
    void ProcessEvent(const SDL_Event& _event);

    // Call at the end of event processing to finalize states
    void EndFrame();

    // Action bindings
    void BindAction(InputAction _action, SDL_Scancode _key);
    bool LoadBindings(const std::string& _path);

    // Query action states
    bool IsActionPressed(InputAction _action) const;   // True only on the frame action was pressed
    bool IsActionHeld(InputAction _action) const;      // True while action is down (including press frame)
    bool IsActionReleased(InputAction _action) const;  // True only on the frame action was released
    bool IsActionDown(InputAction _action) const;      // Alias for IsActionHeld

    ActionState GetActionState(InputAction _action) const;
};
