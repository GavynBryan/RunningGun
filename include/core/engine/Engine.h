#pragma once
#include <memory>
#include <core/framework/GameServiceHost.h>
#include <core/framework/SystemHost.h>
#include <core/input/InputManager.h>
#include <core/platform/PlatformService.h>
#include <core/rendering/RenderContext.h>

class GameMode;
class PrefabService;
class WorldService;
class IRenderMode;
struct Vec2;

class Engine
{
private:
	bool									Quit;

	GameServiceHost							Services;
	SystemHost								Systems;
	InputManager							InputManagerContext;

public:
	void	Run();
	void	QuitGame() { Quit = true; }

	WorldService& GetWorldService();
	GameServiceHost& GetServices();
	InputManager& GetInputManager();
	PrefabService& GetPrefabs();

	void SetGameMode(std::unique_ptr<GameMode> _mode);

	// Create a debug window with independent camera and render mode
	// Returns InvalidRenderContextId on failure
	RenderContextId CreateDebugWindow(
		const WindowConfig& config,
		std::unique_ptr<IRenderMode> renderMode,
		const Vec2& cameraPosition
	);

	// Close a debug window and its render context
	void CloseDebugWindow(RenderContextId contextId);

	Engine();
	~Engine();
};
