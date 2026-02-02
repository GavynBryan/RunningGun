#pragma once

#include <core/framework/IService.h>
#include <core/platform/IPlatformBackend.h>
#include <memory>

class InputService;

// Service that owns and manages the platform backend (SDL, GLFW, native, etc.)
// Provides unified access to: platform lifecycle, window management, and event polling.
// This is the single point of contact for all platform operations.
class PlatformService final : public IService
{
public:
	explicit PlatformService(std::unique_ptr<IPlatformBackend> backend);
	~PlatformService();

	// ===== Platform Info =====
	const char* GetBackendName() const;
	bool IsInitialized() const;

	// ===== Window Management =====
	WindowId CreateWindow(const WindowConfig& config);
	void DestroyWindow(WindowId id);
	WindowHandle* GetWindow(WindowId id);
	const WindowHandle* GetWindow(WindowId id) const;
	WindowHandle* GetPrimaryWindow();
	const WindowHandle* GetPrimaryWindow() const;
	void SetPrimaryWindow(WindowId id);
	bool HasWindow(WindowId id) const;
	size_t GetWindowCount() const;
	bool HasOpenWindows() const;
	void CloseAllWindows();

	// ===== Event Polling =====
	// Poll and process all pending events.
	// Returns true if a quit event was received.
	bool PollEvents();

	// ===== Event Delegates =====
	// Subscribe to platform events via these delegates
	MulticastDelegate<>& OnQuitRequested();
	MulticastDelegate<WindowId>& OnWindowClosed();
	MulticastDelegate<WindowId, int, int>& OnWindowResized();

	// ===== Input Routing =====
	// Connect input service for event routing (call after InputService is registered)
	void SetInputService(InputService* input);

	// ===== Backend Access =====
	// Direct access for platform-specific operations (use sparingly)
	IPlatformBackend& GetBackend() const;

private:
	std::unique_ptr<IPlatformBackend> Backend;
};
