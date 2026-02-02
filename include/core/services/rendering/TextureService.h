#pragma once

#include <core/framework/IService.h>
#include <core/logging/LoggingService.h>
#include <core/rendering/TextureHandle.h>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

class IGraphicsAPI;
class GameServiceHost;

// Service that owns texture resources and manages handle allocation.
// Works with IGraphicsAPI for actual texture upload/storage.
class TextureService final : public IService
{
public:
	explicit TextureService(GameServiceHost& services);
	~TextureService() override;

	// Load a texture from file, returns handle
	TextureHandle Load(const std::string& path);
	
	// Get handle for already-loaded texture by path (returns invalid if not found)
	TextureHandle Get(std::string_view path) const;
	
	// Check if a texture is loaded
	bool IsLoaded(std::string_view path) const;
	
	// Get path for a handle (reverse lookup)
	const std::string* GetPath(TextureHandle handle) const;
	
	// Unload a specific texture
	void Unload(TextureHandle handle);
	
	// Unload all textures
	void UnloadAll();

	// Set the graphics API (called by RenderService on init)
	void SetGraphics(IGraphicsAPI* graphics) { Graphics = graphics; }

	// Upload all loaded textures to a different graphics API
	// Used when creating additional render contexts that need the same textures
	void UploadAllTo(IGraphicsAPI& targetGraphics);

	// Get all loaded texture paths (for iteration)
	std::vector<std::string> GetAllPaths() const;

private:
	TextureHandle AllocateHandle();

	// Path -> Handle mapping
	std::unordered_map<std::string, TextureHandle> PathToHandle;
	// Handle -> Path mapping (for reverse lookup and iteration)
	std::unordered_map<TextureHandle, std::string> HandleToPath;

	IGraphicsAPI* Graphics = nullptr;
	uint32_t NextHandleId = 1;
	Logger Log;
};
