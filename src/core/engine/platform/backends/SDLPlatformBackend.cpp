#include <core/engine/platform/backends/SDLPlatformBackend.h>
#include <core/services/input/InputService.h>

SDLPlatformBackend::~SDLPlatformBackend()
{
	Shutdown();
}

bool SDLPlatformBackend::Init()
{
	if (Initialized) {
		return true;
	}

	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
		SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
		return false;
	}

	Initialized = true;
	return true;
}

void SDLPlatformBackend::Shutdown()
{
	if (!Initialized) {
		return;
	}

	CloseAllWindows();
	SDL_Quit();
	Initialized = false;
}

WindowId SDLPlatformBackend::CreateWindow(const WindowConfig& config)
{
	if (!Initialized) {
		return InvalidWindowId;
	}

	int x = (config.X >= 0) ? config.X : SDL_WINDOWPOS_CENTERED;
	int y = (config.Y >= 0) ? config.Y : SDL_WINDOWPOS_CENTERED;

	SDL_Window* sdlWindow = SDL_CreateWindow(
		config.Title.c_str(),
		config.Width,
		config.Height,
		config.Flags
	);

	if (!sdlWindow) {
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return InvalidWindowId;
	}

	WindowId id = GenerateWindowId();
	
	auto handle = std::make_unique<WindowHandle>();
	handle->NativeHandle = sdlWindow;
	handle->Id = id;
	handle->Title = config.Title;
	handle->Width = config.Width;
	handle->Height = config.Height;
	handle->IsOpen = true;

	SDL_WindowID sdlId = SDL_GetWindowID(sdlWindow);
	SDLIdToWindowId[sdlId] = id;
	Windows[id] = std::move(handle);

	// Set as primary if this is the first window
	if (PrimaryWindowId == InvalidWindowId) {
		PrimaryWindowId = id;
	}

	return id;
}

void SDLPlatformBackend::DestroyWindow(WindowId id)
{
	auto it = Windows.find(id);
	if (it == Windows.end()) {
		return;
	}

	WindowHandle* handle = it->second.get();
	SDL_Window* sdlWindow = static_cast<SDL_Window*>(handle->NativeHandle);
	
	if (sdlWindow) {
		SDL_WindowID sdlId = SDL_GetWindowID(sdlWindow);
		SDLIdToWindowId.erase(sdlId);
		SDL_DestroyWindow(sdlWindow);
	}

	Windows.erase(it);

	// Update primary window if needed
	if (id == PrimaryWindowId) {
		PrimaryWindowId = Windows.empty() ? InvalidWindowId : Windows.begin()->first;
	}
}

WindowHandle* SDLPlatformBackend::GetWindow(WindowId id)
{
	auto it = Windows.find(id);
	return (it != Windows.end()) ? it->second.get() : nullptr;
}

const WindowHandle* SDLPlatformBackend::GetWindow(WindowId id) const
{
	auto it = Windows.find(id);
	return (it != Windows.end()) ? it->second.get() : nullptr;
}

WindowHandle* SDLPlatformBackend::GetPrimaryWindow()
{
	return GetWindow(PrimaryWindowId);
}

const WindowHandle* SDLPlatformBackend::GetPrimaryWindow() const
{
	return GetWindow(PrimaryWindowId);
}

void SDLPlatformBackend::SetPrimaryWindow(WindowId id)
{
	if (HasWindow(id)) {
		PrimaryWindowId = id;
	}
}

bool SDLPlatformBackend::HasWindow(WindowId id) const
{
	return Windows.find(id) != Windows.end();
}

size_t SDLPlatformBackend::GetWindowCount() const
{
	return Windows.size();
}

bool SDLPlatformBackend::HasOpenWindows() const
{
	for (const auto& [id, handle] : Windows) {
		if (handle->IsOpen) {
			return true;
		}
	}
	return false;
}

void SDLPlatformBackend::CloseAllWindows()
{
	std::vector<WindowId> ids;
	for (const auto& [id, handle] : Windows) {
		ids.push_back(id);
	}
	for (WindowId id : ids) {
		DestroyWindow(id);
	}
}

bool SDLPlatformBackend::PollEvents()
{
	SDL_Event event;
	bool quitRequested = false;

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_EVENT_QUIT:
				quitRequested = true;
				OnQuitRequested.Broadcast();
				break;

			case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
				ProcessWindowEvent(event);
				break;

			case SDL_EVENT_WINDOW_RESIZED:
				ProcessWindowEvent(event);
				break;

			case SDL_EVENT_KEY_DOWN:
			case SDL_EVENT_KEY_UP:
			case SDL_EVENT_MOUSE_BUTTON_DOWN:
			case SDL_EVENT_MOUSE_BUTTON_UP:
			case SDL_EVENT_MOUSE_MOTION:
				if (Input) {
					Input->ProcessEvent(event);
				}
				break;

			default:
				break;
		}
	}

	return quitRequested;
}

WindowHandle* SDLPlatformBackend::GetWindowBySDLId(SDL_WindowID sdlId)
{
	auto it = SDLIdToWindowId.find(sdlId);
	if (it == SDLIdToWindowId.end()) {
		return nullptr;
	}
	return GetWindow(it->second);
}

WindowId SDLPlatformBackend::GenerateWindowId()
{
	return NextWindowId++;
}

void SDLPlatformBackend::ProcessWindowEvent(const SDL_Event& event)
{
	SDL_WindowID sdlId = event.window.windowID;
	auto idIt = SDLIdToWindowId.find(sdlId);
	if (idIt == SDLIdToWindowId.end()) {
		return;
	}

	WindowId windowId = idIt->second;
	WindowHandle* handle = GetWindow(windowId);
	if (!handle) {
		return;
	}

	switch (event.type) {
		case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
			handle->IsOpen = false;
			OnWindowClosed.Broadcast(windowId);
			DestroyWindow(windowId);
			break;

		case SDL_EVENT_WINDOW_RESIZED:
			handle->Width = event.window.data1;
			handle->Height = event.window.data2;
			OnWindowResized.Broadcast(windowId, handle->Width, handle->Height);
			break;

		default:
			break;
	}
}
