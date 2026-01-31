#pragma once

class InputManager;
union SDL_Event;

class InputService final
{
public:
	explicit InputService(InputManager& input);

	InputManager& GetInput() const;

	void BeginFrame();
	void ProcessEvent(const SDL_Event& event);
	void EndFrame();

private:
	InputManager& Input;
};
