#include <core/engine/Engine.h>
#include <core/gameplay/GameMode.h>
#include <core/gameplay/GameModeService.h>
#include <core/logging/LoggingService.h>
#include <core/rendering/Camera.h>
#include <core/rendering/TextureService.h>
#include <core/rendering/RenderSystem.h>
#include <core/rendering/RenderContextService.h>
#include <core/rendering/RenderableRegistry.h>
#include <core/rendering/IRenderable.h>
#include <core/rendering/IRenderMode.h>
#include <core/platform/PlatformService.h>
#include <core/platform/backends/SDLPlatformBackend.h>
#include <core/rendering/backends/SDL3Renderer.h>
#include <core/rendering/modes/RenderMode2D.h>
#include <core/collision/CollisionService.h>
#include <core/input/InputService.h>
#include <core/world/ObjectPoolService.h>
#include <core/collision/PhysicsService.h>
#include <core/timing/TimeService.h>
#include <core/world/WorldService.h>
#include <core/actor/ActorService.h>
#include <core/timing/ClockSystem.h>
#include <core/collision/CollisionResponseSystem.h>
#include <core/timing/TimerSystem.h>
#include <core/collision/QuadTreeCollisionSystem.h>
#include <core/world/WorldSystem.h>
#include <core/actor/ActorSystem.h>
#include <core/engine/framework/SystemOrder.h>
#include <core/entity/PrefabService.h>
#include <core/math/Vec2.h>

Engine::Engine()
	:Quit(false)
{
	// Initialize logging first so it's available to all other services
	Services.AddService<LoggingService>();

	// Initialize platform (SDL handles video, input, audio, windows, events)
	Services.AddService<PlatformService>(std::make_unique<SDLPlatformBackend>());
	auto& platform = Services.Get<PlatformService>();

	// Create the primary window
	WindowConfig mainWindowConfig;
	mainWindowConfig.Title = "Running Gun";
	mainWindowConfig.Width = 800;
	mainWindowConfig.Height = 600;

	WindowId mainWindowId = platform.CreateWindow(mainWindowConfig);
	if (mainWindowId == InvalidWindowId) {
		SDL_Log("Failed to create main window");
		return;
	}

	// Register services
	Services.AddService<TimeService>();
	Services.AddService<InputService>(InputManagerContext);
	Services.AddService<PhysicsService>();
	Services.AddService<CollisionService>();
	Services.AddService<TextureService>();
	Services.AddService<RenderContextService>(Services.Get<LoggingService>());
	Services.AddService<RenderableRegistry>();
	Services.RegisterInstanceRegistry<IRenderable>(Services.Get<RenderableRegistry>());
	Services.AddService<WorldService>();
	// Note: Components now self-register via ComponentFactory singleton - no service needed
	Services.AddService<PrefabService>();
	Services.AddService<ObjectPoolService>();
	Services.AddService<GameModeService>();

	// Connect platform to input service for event routing
	platform.SetInputService(&Services.Get<InputService>());

	// Subscribe to window close events to clean up render contexts
	auto& contextService = Services.Get<RenderContextService>();
	platform.OnWindowClosed().Subscribe([&contextService](WindowId windowId) {
		contextService.OnWindowClosed(windowId);
	});

	// Create primary render context for the main window
	WindowHandle* mainWindow = platform.GetPrimaryWindow();
	auto renderer = std::make_unique<SDL3Renderer>(static_cast<SDL_Window*>(mainWindow->NativeHandle));
	auto renderMode = std::make_unique<RenderMode2D>();
	Camera mainCamera(800.0f, 600.0f);

	RenderContextId mainContextId = contextService.CreateContext(
		mainWindowId,
		std::move(renderer),
		std::move(renderMode),
		mainCamera,
		"Main"
	);

	if (mainContextId == InvalidRenderContextId) {
		SDL_Log("Failed to create main render context");
		return;
	}

	// Connect texture service to primary context's graphics API
	RenderContext* mainContext = contextService.GetPrimaryContext();
	Services.Get<TextureService>().SetGraphics(mainContext->Graphics.get());

	// Register systems (logic - order matters)
	Systems.AddSystem<ClockSystem>(SystemOrder::Clock, Services);
	Systems.AddSystem<TimerSystem>(SystemOrder::Timer, Services);
	Systems.AddSystem<QuadTreeCollisionSystem>(SystemOrder::Collision, Services);
	Systems.AddSystem<CollisionResponseSystem>(SystemOrder::CollisionResponse, Services);
	Systems.AddSystem<WorldSystem>(SystemOrder::World, Services);
	Systems.AddSystem<RenderSystem>(SystemOrder::Render, Services);
}

Engine::~Engine()
{
	// Services are destroyed in reverse order of registration
	// PlatformService destructor calls SDL_Quit()
}

void Engine::Run()
{
	if (!Services.Get<GameModeService>().GetCurrentMode()) {
		SDL_Log("Engine cannot run without a game mode.");
		return;
	}
	Systems.Init();

	auto& platform = Services.Get<PlatformService>();

	while (!Quit && platform.HasOpenWindows()) {
		// Poll and process all platform events
		if (platform.PollEvents()) {
			Quit = true;
		}

		// Update systems (RenderSystem calls RenderContextService::RenderAllContexts)
		Systems.Update();

		// Frame rate limiting (approximately 120 FPS)
		SDL_Delay(8);
	}

	Systems.Shutdown();
}

WorldService& Engine::GetWorldService()
{
	return Services.Get<WorldService>();
}

GameServiceHost& Engine::GetServices()
{
	return Services;
}

InputManager& Engine::GetInputManager()
{
	return InputManagerContext;
}

PrefabService& Engine::GetPrefabs()
{
	return Services.Get<PrefabService>();
}

void Engine::SetGameMode(std::unique_ptr<GameMode> _mode)
{
	Services.Get<GameModeService>().SetGameMode(std::move(_mode));
}

RenderContextId Engine::CreateDebugWindow(
	const WindowConfig& config,
	std::unique_ptr<IRenderMode> renderMode,
	const Vec2& cameraPosition)
{
	auto& platform = Services.Get<PlatformService>();
	auto& contextService = Services.Get<RenderContextService>();
	auto& textureService = Services.Get<TextureService>();

	// Create the window
	WindowId windowId = platform.CreateWindow(config);
	if (windowId == InvalidWindowId) {
		SDL_Log("Failed to create debug window");
		return InvalidRenderContextId;
	}

	// Get the window handle
	WindowHandle* window = platform.GetWindow(windowId);
	if (!window) {
		SDL_Log("Failed to get debug window handle");
		return InvalidRenderContextId;
	}

	// Create renderer for this window
	auto renderer = std::make_unique<SDL3Renderer>(static_cast<SDL_Window*>(window->NativeHandle));

	// Set up camera at the specified position
	Camera debugCamera(static_cast<float>(config.Width), static_cast<float>(config.Height));
	debugCamera.SetPosition(cameraPosition);

	// Create the render context
	RenderContextId contextId = contextService.CreateContext(
		windowId,
		std::move(renderer),
		std::move(renderMode),
		debugCamera,
		config.Title
	);

	if (contextId == InvalidRenderContextId) {
		SDL_Log("Failed to create debug render context");
		platform.DestroyWindow(windowId);
		return InvalidRenderContextId;
	}

	// Sync textures from primary context to the new debug context
	// SDL textures are renderer-specific, so we need to duplicate them
	RenderContext* primaryContext = contextService.GetPrimaryContext();
	RenderContext* debugContext = contextService.GetContext(contextId);
	if (primaryContext && primaryContext->Graphics && debugContext && debugContext->Graphics) {
		contextService.SyncTextures(*primaryContext->Graphics, *debugContext->Graphics, textureService);
	}

	return contextId;
}

void Engine::CloseDebugWindow(RenderContextId contextId)
{
	auto& contextService = Services.Get<RenderContextService>();
	auto& platform = Services.Get<PlatformService>();

	RenderContext* context = contextService.GetContext(contextId);
	if (!context) {
		return;
	}

	// Don't allow closing the primary context this way
	if (contextId == contextService.GetPrimaryContextId()) {
		SDL_Log("Cannot close primary render context via CloseDebugWindow");
		return;
	}

	WindowId windowId = context->Window;
	
	// Destroy the context first (shuts down renderer)
	contextService.DestroyContext(contextId);
	
	// Then destroy the window
	platform.DestroyWindow(windowId);
}
