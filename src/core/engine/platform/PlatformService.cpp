#include <core/platform/PlatformService.h>
#include <stdexcept>

PlatformService::PlatformService(std::unique_ptr<IPlatformBackend> backend)
	: Backend(std::move(backend))
{
	if (Backend) {
		Backend->Init();
	}
}

PlatformService::~PlatformService()
{
	if (Backend) {
		Backend->Shutdown();
	}
}

// ===== Platform Info =====

const char* PlatformService::GetBackendName() const
{
	return Backend ? Backend->GetName() : "None";
}

bool PlatformService::IsInitialized() const
{
	return Backend != nullptr;
}

// ===== Window Management =====

WindowId PlatformService::CreateWindow(const WindowConfig& config)
{
	if (!Backend) return InvalidWindowId;
	return Backend->CreateWindow(config);
}

void PlatformService::DestroyWindow(WindowId id)
{
	if (!Backend) return;
	Backend->DestroyWindow(id);
}

WindowHandle* PlatformService::GetWindow(WindowId id)
{
	if (!Backend) return nullptr;
	return Backend->GetWindow(id);
}

const WindowHandle* PlatformService::GetWindow(WindowId id) const
{
	if (!Backend) return nullptr;
	return Backend->GetWindow(id);
}

WindowHandle* PlatformService::GetPrimaryWindow()
{
	if (!Backend) return nullptr;
	return Backend->GetPrimaryWindow();
}

const WindowHandle* PlatformService::GetPrimaryWindow() const
{
	if (!Backend) return nullptr;
	return Backend->GetPrimaryWindow();
}

void PlatformService::SetPrimaryWindow(WindowId id)
{
	if (!Backend) return;
	Backend->SetPrimaryWindow(id);
}

bool PlatformService::HasWindow(WindowId id) const
{
	if (!Backend) return false;
	return Backend->HasWindow(id);
}

size_t PlatformService::GetWindowCount() const
{
	if (!Backend) return 0;
	return Backend->GetWindowCount();
}

bool PlatformService::HasOpenWindows() const
{
	if (!Backend) return false;
	return Backend->HasOpenWindows();
}

void PlatformService::CloseAllWindows()
{
	if (!Backend) return;
	Backend->CloseAllWindows();
}

// ===== Event Polling =====

bool PlatformService::PollEvents()
{
	if (!Backend) return true; // Return true (quit) if no backend
	return Backend->PollEvents();
}

// ===== Event Delegates =====

MulticastDelegate<>& PlatformService::OnQuitRequested()
{
	static MulticastDelegate<> EmptyDelegate;
	if (!Backend) return EmptyDelegate;
	return Backend->OnQuitRequested;
}

MulticastDelegate<WindowId>& PlatformService::OnWindowClosed()
{
	static MulticastDelegate<WindowId> EmptyDelegate;
	if (!Backend) return EmptyDelegate;
	return Backend->OnWindowClosed;
}

MulticastDelegate<WindowId, int, int>& PlatformService::OnWindowResized()
{
	static MulticastDelegate<WindowId, int, int> EmptyDelegate;
	if (!Backend) return EmptyDelegate;
	return Backend->OnWindowResized;
}

// ===== Input Routing =====

void PlatformService::SetInputService(InputService* input)
{
	if (Backend) {
		Backend->SetInputService(input);
	}
}

// ===== Backend Access =====

IPlatformBackend& PlatformService::GetBackend() const
{
	// This is the only method that still asserts - direct backend access 
	// should only happen when you're certain the backend exists
	if (!Backend) {
		throw std::runtime_error("PlatformService::GetBackend() called with no backend");
	}
	return *Backend;
}
