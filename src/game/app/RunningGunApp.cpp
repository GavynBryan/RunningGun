#include <game/app/RunningGunApp.h>
#include <core/engine/Engine.h>
#include <core/input/InputManager.h>
#include <core/rendering/TextureService.h>
#include <core/entity/ComponentRegistryService.h>
#include <core/entity/PrefabService.h>
#include <game/RunningGunGameMode.h>
#include <game/components/ComponentRegistration.h>
#include <game/input/PlayerInputConfig.h>
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
	RegisterDefaultComponents(services.Get<ComponentRegistryService>(), _inputConfig);
	services.Get<PrefabService>().LoadFromFile("config/prefabs.json", services.Get<TextureService>());

	_engine.SetGameMode(std::make_unique<RunningGunGameMode>(services));
	_engine.Run();
	return EXIT_SUCCESS;
}
