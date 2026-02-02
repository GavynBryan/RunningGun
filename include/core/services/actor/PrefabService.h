#pragma once

#include <core/entity/Entity.h>
#include <core/framework/IService.h>
#include <core/math/Vec2.h>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

class ComponentRegistryService;
class TextureService;
class AnimationClipLibrary;

struct AnimationDefinition
{
	std::string Name;
	int Index = 0;
	Vec2 FrameSize = Vec2(0, 0);
	int Frames = 0;
	bool Loop = false;
	bool Priority = false;
};

struct ComponentDefinition
{
	std::string Type;
	std::string ParamsJson;
};

struct PrefabDefinition
{
	std::string Id;
	std::string Texture;
	float Width = 0.0f;
	float Height = 0.0f;
	Vec2 Position = Vec2(0, 0);
	ENTITY_TAG Tag = player;
	
	// Animation configuration - use ONE of these approaches:
	// 1. Reference a shared animation set by name (preferred for reuse)
	std::string AnimationSet;
	// 2. Define animations inline (creates a unique set for this prefab)
	std::vector<AnimationDefinition> Animations;
	
	std::vector<ComponentDefinition> Components;
};

// Service that manages prefab definitions and entity instantiation.
// Loads prefab configurations from JSON and creates entities with
// their associated components, sprites, and animations.
class PrefabService final : public IService
{
public:
	PrefabService() = default;
	~PrefabService() override = default;

	// Load prefab definitions from a JSON file
	bool LoadFromFile(const std::string& path);
	
	// Load prefabs and automatically load referenced textures
	bool LoadFromFile(const std::string& path, TextureService& textures);

	// Find a prefab definition by ID
	const PrefabDefinition* Find(std::string_view id) const;

	// Create an entity from a prefab definition
	std::unique_ptr<Actor> Instantiate(std::string_view id) const;
	std::unique_ptr<Actor> Instantiate(const PrefabDefinition& definition) const;

	// Get all texture paths referenced by loaded prefabs
	std::vector<std::string> GetTexturePaths() const;

	// Register animation sets from loaded prefabs into the library.
	// - Prefabs with inline animations create a set named after the prefab ID
	// - Prefabs referencing an animationSet don't create new sets
	// Call this after LoadFromFile to enable shared clip data.
	void RegisterAnimationSets(AnimationClipLibrary& library) const;
	
	size_t Count() const;
	void Clear();

private:
	std::unordered_map<std::string, PrefabDefinition> Definitions;
};
