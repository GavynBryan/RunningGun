#include <core/rendering/TextureService.h>
#include <core/rendering/IGraphicsAPI.h>
#include <core/framework/GameServiceHost.h>
#include <cassert>

TextureService::TextureService(GameServiceHost& services)
	: Log(services.Get<LoggingService>(), "TextureService")
{
}

TextureService::~TextureService()
{
	UnloadAll();
}

TextureHandle TextureService::Load(const std::string& path)
{
	// Check if already loaded
	auto it = PathToHandle.find(path);
	if (it != PathToHandle.end()) {
		return it->second;
	}

	if (!Graphics) {
		Log.Error("Cannot load texture - graphics API not set");
		return TextureHandle::Invalid();
	}

	// Allocate new handle
	TextureHandle handle = AllocateHandle();

	// Upload to graphics API
	if (!Graphics->UploadTextureFromFile(handle, path)) {
		Log.Error("Failed to load texture: " + path);
		return TextureHandle::Invalid();
	}

	// Store mappings
	PathToHandle[path] = handle;
	HandleToPath[handle] = path;

	return handle;
}

TextureHandle TextureService::Get(std::string_view path) const
{
	auto it = PathToHandle.find(std::string(path));
	if (it != PathToHandle.end()) {
		return it->second;
	}
	return TextureHandle::Invalid();
}

bool TextureService::IsLoaded(std::string_view path) const
{
	return PathToHandle.find(std::string(path)) != PathToHandle.end();
}

const std::string* TextureService::GetPath(TextureHandle handle) const
{
	auto it = HandleToPath.find(handle);
	if (it != HandleToPath.end()) {
		return &it->second;
	}
	return nullptr;
}

void TextureService::Unload(TextureHandle handle)
{
	auto pathIt = HandleToPath.find(handle);
	if (pathIt == HandleToPath.end()) {
		return;
	}

	const std::string& path = pathIt->second;

	if (Graphics) {
		Graphics->DestroyTexture(handle);
	}

	PathToHandle.erase(path);
	HandleToPath.erase(pathIt);
}

void TextureService::UnloadAll()
{
	if (Graphics) {
		for (const auto& [handle, path] : HandleToPath) {
			Graphics->DestroyTexture(handle);
		}
	}

	PathToHandle.clear();
	HandleToPath.clear();
}

void TextureService::UploadAllTo(IGraphicsAPI& targetGraphics)
{
	// Upload all loaded textures to the target graphics API
	// This duplicates textures for multi-renderer support
	for (const auto& [handle, path] : HandleToPath) {
		if (!targetGraphics.UploadTextureFromFile(handle, path)) {
			Log.Warning("UploadAllTo - Failed to upload texture: " + path);
		}
	}
}

std::vector<std::string> TextureService::GetAllPaths() const
{
	std::vector<std::string> paths;
	paths.reserve(HandleToPath.size());
	for (const auto& [handle, path] : HandleToPath) {
		paths.push_back(path);
	}
	return paths;
}

TextureHandle TextureService::AllocateHandle()
{
	return TextureHandle{ NextHandleId++ };
}
