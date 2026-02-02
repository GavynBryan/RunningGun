#include <app/RunningGunApp.h>
#include <core/engine/Engine.h>
#include <core/input/InputManager.h>
#include <core/rendering/TextureService.h>
#include <core/entity/PrefabService.h>
#include <game/RunningGunGameMode.h>
#include <game/input/PlayerInputConfig.h>
#include <game/input/PlayerInputConfigService.h>
#include <cstdlib>
#include <memory>
#include <unordered_map>

int RunningGunApp::Run()
{
	Engine _engine;
	PlayerInputConfig _inputConfig;
	std::unordered_map<std::string, SDL_Scancode> _bindings;

	if (InputManager::LoadKeyBindings("config/controls.json", _bindings)) {
		_inputConfig.ApplyBindings(_bindings);
	}

	auto& services = _engine.GetServices();
	
	// Register input config as a service so components can access it
	services.Register<PlayerInputConfigService>(std::move(_inputConfig));
	
	// Components now self-register via COMPONENT macro - no manual registration needed
	services.Get<PrefabService>().LoadFromFile("config/prefabs.json", services.Get<TextureService>());

	_engine.SetGameMode(std::make_unique<RunningGunGameMode>(services));
	_engine.Run();
	return EXIT_SUCCESS;
}
