#pragma once

#include <cstdint>
#include <functional>

// Opaque handle for textures - decoupled from any specific rendering backend
struct TextureHandle
{
	uint32_t Id = 0;

	bool IsValid() const { return Id != 0; }
	bool operator==(const TextureHandle& other) const { return Id == other.Id; }
	bool operator!=(const TextureHandle& other) const { return Id != other.Id; }

	static TextureHandle Invalid() { return TextureHandle{0}; }
};

// Hash support for unordered containers
template<>
struct std::hash<TextureHandle>
{
	std::size_t operator()(const TextureHandle& handle) const noexcept
	{
		return std::hash<uint32_t>{}(handle.Id);
	}
};
