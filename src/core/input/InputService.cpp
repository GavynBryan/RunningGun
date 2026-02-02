#include <core/input/InputService.h>
#include <core/input/InputManager.h>

InputService::InputService(InputManager& input)
	: Input(input)
{
}

InputManager& InputService::GetInput() const
{
	return Input;
}

void InputService::BeginFrame()
{
	Input.BeginFrame();
}

void InputService::ProcessEvent(const SDL_Event& event)
{
	Input.ProcessEvent(event);
}

void InputService::EndFrame()
{
	Input.EndFrame();
}
