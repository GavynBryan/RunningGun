#include <game/app/RunningGunApp.h>
#include <core/engine/Engine.h>
#include <core/InputManager.h>
#include <core/engine/InputService.h>
#include <core/engine/RenderService.h>
#include <game/RunningGunGameMode.h>
#include <game/components/ComponentRegistration.h>
#include <game/input/PlayerInputConfig.h>
#include <cstdlib>
#include <memory>
#include <unordered_map>

int RunningGunApp::Run()
{
	Engine _engine;

	// Configure player input bindings via the InputService
	std::unordered_map<std::string, SDL_Scancode> _bindings;
	if (InputManager::LoadKeyBindings("config/controls.json", _bindings)) {
		PlayerInputConfig _inputConfig;
		_inputConfig.ApplyBindings(_bindings);
		_engine.GetServices().Get<InputService>().SetPlayerInputConfig(_inputConfig);
	}

	RegisterDefaultComponents(_engine.GetPrefabs().GetRegistry());
	_engine.GetPrefabs().LoadFromFile("config/prefabs.json", _engine.GetServices().Get<RenderService>().GetTextureHandler());

	_engine.SetGameMode(std::make_unique<RunningGunGameMode>(
		_engine.GetRenderer(),
		_engine.GetServices(),
		_engine.GetPrefabs(),
		_engine.GetWorld()));

	_engine.Run();
	return EXIT_SUCCESS;
}
