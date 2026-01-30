#include <game/app/RunningGunApp.h>
#include <core/engine/Engine.h>
#include <game/RunningGunGameMode.h>
#include <game/components/ComponentRegistration.h>
#include <game/input/PlayerInputConfig.h>
#include <cstdlib>
#include <memory>

int RunningGunApp::Run()
{
	Engine _engine;
	PlayerInputConfig _inputConfig;

	_inputConfig.LoadBindings("config/controls.json");

	RegisterDefaultComponents(_engine.GetPrefabs().GetRegistry(), _inputConfig);
	_engine.GetPrefabs().LoadFromFile("config/prefabs.json", _engine.GetServices().GetTextureHandler());

	_engine.SetGameMode(std::make_unique<RunningGunGameMode>(
		_engine.GetRenderer(),
		_engine.GetServices(),
		_engine.GetPrefabs(),
		_engine.GetWorld()));

	_engine.Run();
	return EXIT_SUCCESS;
}
