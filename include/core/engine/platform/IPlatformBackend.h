#pragma once

#include <core/events/MulticastDelegate.h>
#include <cstdint>
#include <string>

// Forward declarations - platform-agnostic types
using WindowId = uint32_t;
constexpr WindowId InvalidWindowId = 0;

class InputService;

// Platform-agnostic window configuration
struct WindowConfig
{
	std::string Title = "Window";
	int Width = 800;
	int Height = 600;
	uint32_t Flags = 0;
	int X = -1;  // -1 = centered
	int Y = -1;  // -1 = centered
};

// Platform-agnostic window handle
// NativeHandle is void* to avoid leaking platform types into the interface
struct WindowHandle
{
	void* NativeHandle = nullptr;
	WindowId Id = InvalidWindowId;
	std::string Title;
	int Width = 0;
	int Height = 0;
	bool IsOpen = false;
};

// Abstract interface for platform backends.
// Consolidates platform initialization, window management, and event polling.
// Implementations: SDLPlatformBackend, GLFWPlatformBackend, Win32PlatformBackend, etc.
class IPlatformBackend
{
public:
	virtual ~IPlatformBackend() = default;

	// ===== Lifecycle =====
	virtual bool Init() = 0;
	virtual void Shutdown() = 0;
	virtual const char* GetName() const = 0;

	// ===== Window Management =====
	virtual WindowId CreateWindow(const WindowConfig& config) = 0;
	virtual void DestroyWindow(WindowId id) = 0;
	virtual WindowHandle* GetWindow(WindowId id) = 0;
	virtual const WindowHandle* GetWindow(WindowId id) const = 0;
	virtual WindowHandle* GetPrimaryWindow() = 0;
	virtual const WindowHandle* GetPrimaryWindow() const = 0;
	virtual void SetPrimaryWindow(WindowId id) = 0;
	virtual bool HasWindow(WindowId id) const = 0;
	virtual size_t GetWindowCount() const = 0;
	virtual bool HasOpenWindows() const = 0;
	virtual void CloseAllWindows() = 0;

	// ===== Event Polling =====
	// Poll all pending events and dispatch to handlers.
	// Returns true if a quit event was received.
	virtual bool PollEvents() = 0;

	// ===== Input Routing =====
	// Set input service for event routing (optional, can be null)
	virtual void SetInputService(InputService* input) = 0;

	// Delegate fired when quit is requested (window close, platform quit, etc.)
	MulticastDelegate<> OnQuitRequested;

	// Delegate fired when a window is closed (passes the WindowId)
	MulticastDelegate<WindowId> OnWindowClosed;

	// Delegate fired when a window is resized (passes WindowId, new width, new height)
	MulticastDelegate<WindowId, int, int> OnWindowResized;
};
