#pragma once

#include <core/services/component/ComponentFactory.h>
#include <core/entity/Entity.h>
#include <core/math/Vec2.h>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

class GameServiceHost;
class TextureService;

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
	void SetServices(GameServiceHost& services);

	ComponentFactory& GetFactory();
	const ComponentFactory& GetFactory() const;

	bool LoadFromFile(const std::string& path);
	bool LoadFromFile(const std::string& path, TextureService& textures);

	const PrefabDefinition* Find(std::string_view id) const;

	std::unique_ptr<Entity> Instantiate(std::string_view id) const;
	std::unique_ptr<Entity> Instantiate(const PrefabDefinition& definition) const;

	std::vector<std::string> GetTexturePaths() const;
	size_t Count() const;
	void Clear();

private:
	ComponentFactory Factory;
	std::unordered_map<std::string, PrefabDefinition> Definitions;
	GameServiceHost* Services = nullptr;
};
