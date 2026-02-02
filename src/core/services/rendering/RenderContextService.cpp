#include <core/rendering/RenderContextService.h>
#include <core/rendering/IGraphicsAPI.h>
#include <core/rendering/IRenderMode.h>
#include <core/rendering/TextureService.h>
#include <string>

RenderContextService::RenderContextService(LoggingService& logging)
	: Log(logging, "RenderContextService")
{
}

RenderContextService::~RenderContextService()
{
	// Contexts are automatically cleaned up via unique_ptr
	Contexts.clear();
	WindowToContext.clear();
}

RenderContextId RenderContextService::CreateContext(
	WindowId window,
	std::unique_ptr<IGraphicsAPI> graphics,
	std::unique_ptr<IRenderMode> renderMode,
	const Camera& camera,
	const std::string& name)
{
	if (window == InvalidWindowId)
	{
		Log.Error("CreateContext - Invalid window ID");
		return InvalidRenderContextId;
	}

	if (!graphics)
	{
		Log.Error("CreateContext - Graphics API is null");
		return InvalidRenderContextId;
	}

	if (!renderMode)
	{
		Log.Warning("CreateContext - Render mode is null, context will not render");
	}

	// Initialize the graphics API
	if (!graphics->Init())
	{
		Log.Error("CreateContext - Failed to initialize graphics API");
		return InvalidRenderContextId;
	}

	RenderContextId id = GenerateContextId();

	auto context = std::make_unique<RenderContext>();
	context->Id = id;
	context->Window = window;
	context->Graphics = std::move(graphics);
	context->RenderMode = std::move(renderMode);
	context->ContextCamera = camera;
	context->Name = name.empty() ? ("Context " + std::to_string(id)) : name;
	context->IsEnabled = true;

	WindowToContext[window] = id;
	Contexts[id] = std::move(context);

	// First context becomes primary
	if (PrimaryContextId == InvalidRenderContextId)
	{
		PrimaryContextId = id;
	}

	return id;
}

void RenderContextService::DestroyContext(RenderContextId id)
{
	auto it = Contexts.find(id);
	if (it == Contexts.end())
	{
		return;
	}

	RenderContext* context = it->second.get();
	
	// Remove window mapping
	WindowToContext.erase(context->Window);

	// Shutdown graphics
	if (context->Graphics)
	{
		context->Graphics->Shutdown();
	}

	// Update primary if we destroyed it
	if (PrimaryContextId == id)
	{
		PrimaryContextId = InvalidRenderContextId;
		// Find another context to be primary
		for (const auto& [otherId, otherContext] : Contexts)
		{
			if (otherContext && otherContext->IsEnabled && otherId != id)
			{
				PrimaryContextId = otherId;
				break;
			}
		}
	}

	Contexts.erase(it);
}

RenderContext* RenderContextService::GetContext(RenderContextId id)
{
	auto it = Contexts.find(id);
	return (it != Contexts.end()) ? it->second.get() : nullptr;
}

const RenderContext* RenderContextService::GetContext(RenderContextId id) const
{
	auto it = Contexts.find(id);
	return (it != Contexts.end()) ? it->second.get() : nullptr;
}

RenderContext* RenderContextService::GetContextByWindow(WindowId windowId)
{
	auto it = WindowToContext.find(windowId);
	if (it == WindowToContext.end())
	{
		return nullptr;
	}
	return GetContext(it->second);
}

RenderContext* RenderContextService::GetPrimaryContext()
{
	return GetContext(PrimaryContextId);
}

const RenderContext* RenderContextService::GetPrimaryContext() const
{
	return GetContext(PrimaryContextId);
}

void RenderContextService::SetPrimaryContext(RenderContextId id)
{
	if (HasContext(id))
	{
		PrimaryContextId = id;
	}
}

bool RenderContextService::HasContext(RenderContextId id) const
{
	auto it = Contexts.find(id);
	return it != Contexts.end() && it->second && it->second->IsEnabled;
}

size_t RenderContextService::GetContextCount() const
{
	size_t count = 0;
	for (const auto& [id, context] : Contexts)
	{
		if (context && context->IsEnabled)
		{
			++count;
		}
	}
	return count;
}

void RenderContextService::RenderAllContexts(BatchArray<IRenderable>& renderables)
{
	for (auto& [id, context] : Contexts)
	{
		if (!context || !context->IsEnabled || !context->Graphics)
		{
			continue;
		}

		IGraphicsAPI& graphics = *context->Graphics;
		Camera& camera = context->ContextCamera;

		// Begin frame for this context
		graphics.BeginFrame();
		graphics.Clear(Color(0, 0, 0, 255));

		// Set camera on graphics API (Camera is the single source of truth)
		graphics.SetCamera(camera.GetCameraData());

		// Render using context's render mode (if set)
		if (context->RenderMode)
		{
			context->RenderMode->RenderScene(graphics, renderables, camera);
		}

		// End frame and present
		graphics.EndFrame();
		graphics.Present();
	}
}

void RenderContextService::OnWindowClosed(WindowId windowId)
{
	auto it = WindowToContext.find(windowId);
	if (it != WindowToContext.end())
	{
		DestroyContext(it->second);
	}
}

void RenderContextService::SyncTextures(IGraphicsAPI& source, IGraphicsAPI& target, TextureService& textures)
{
	if (&source == &target)
	{
		return;
	}

	Log.Info(std::string("SyncTextures - Syncing textures from '") + source.GetName() + 
		"' to '" + target.GetName() + "'");

	textures.UploadAllTo(target);
}

RenderContextId RenderContextService::GenerateContextId()
{
	return NextContextId++;
}
