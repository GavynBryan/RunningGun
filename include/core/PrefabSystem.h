#pragma once

#include <core/ComponentRegistry.h>
#include <core/Entity.h>
#include <core/Vec2.h>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

class EngineServices;
class ResourceHandler;

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
	std::vector<AnimationDefinition> Animations;
	std::vector<ComponentDefinition> Components;
};

class PrefabSystem
{
public:
	ComponentRegistry& GetRegistry();
	const ComponentRegistry& GetRegistry() const;

	bool LoadFromFile(const std::string& path);
	bool LoadFromFile(const std::string& path, ResourceHandler& textures);

	const PrefabDefinition* Find(std::string_view id) const;

	std::unique_ptr<Entity> Instantiate(std::string_view id, EngineServices& services) const;
	std::unique_ptr<Entity> Instantiate(const PrefabDefinition& definition, EngineServices& services) const;

	std::vector<std::string> GetTexturePaths() const;
	size_t Count() const;
	void Clear();

private:
	ComponentRegistry Registry;
	std::unordered_map<std::string, PrefabDefinition> Definitions;
};
