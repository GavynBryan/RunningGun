#pragma once

#include <core/engine/IService.h>
#include <game/input/PlayerInputConfig.h>

class InputManager;
union SDL_Event;

class InputService final : public IService
{
public:
	explicit InputService(InputManager& input);

	InputManager& GetInput() const;
	const PlayerInputConfig& GetPlayerInputConfig() const { return InputConfig; }
	void SetPlayerInputConfig(const PlayerInputConfig& config) { InputConfig = config; }

	void BeginFrame();
	void ProcessEvent(const SDL_Event& event);
	void EndFrame();

private:
	InputManager& Input;
	PlayerInputConfig InputConfig;
};
