#pragma once

#include <core/platform/IPlatformBackend.h>
#include <SDL3/SDL.h>
#include <unordered_map>
#include <memory>

class InputService;

// SDL3-based platform backend implementation.
// Handles SDL initialization, window management, and event polling.
class SDLPlatformBackend final : public IPlatformBackend
{
public:
	SDLPlatformBackend() = default;
	~SDLPlatformBackend() override;

	// ===== Lifecycle =====
	bool Init() override;
	void Shutdown() override;
	const char* GetName() const override { return "SDL3"; }

	// ===== Window Management =====
	WindowId CreateWindow(const WindowConfig& config) override;
	void DestroyWindow(WindowId id) override;
	WindowHandle* GetWindow(WindowId id) override;
	const WindowHandle* GetWindow(WindowId id) const override;
	WindowHandle* GetPrimaryWindow() override;
	const WindowHandle* GetPrimaryWindow() const override;
	void SetPrimaryWindow(WindowId id) override;
	bool HasWindow(WindowId id) const override;
	size_t GetWindowCount() const override;
	bool HasOpenWindows() const override;
	void CloseAllWindows() override;

	// ===== Event Polling =====
	bool PollEvents() override;

	// ===== Input Routing =====
	void SetInputService(InputService* input) override { Input = input; }

	// Get window by SDL window ID (for internal event handling)
	WindowHandle* GetWindowBySDLId(SDL_WindowID sdlId);

private:
	WindowId GenerateWindowId();
	void ProcessWindowEvent(const SDL_Event& event);

	bool Initialized = false;
	InputService* Input = nullptr;

	std::unordered_map<WindowId, std::unique_ptr<WindowHandle>> Windows;
	std::unordered_map<SDL_WindowID, WindowId> SDLIdToWindowId;
	WindowId PrimaryWindowId = InvalidWindowId;
	WindowId NextWindowId = 1;
};
