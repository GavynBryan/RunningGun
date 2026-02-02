#pragma once

#include <core/framework/IService.h>

class InputManager;
union SDL_Event;

class InputService final : public IService
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
