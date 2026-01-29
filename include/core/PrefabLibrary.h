#pragma once

#include <core/Entity.h>
#include <core/Vec2.h>
#include <rapidjson/document.h>
#include <string>
#include <unordered_map>
#include <vector>

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
	rapidjson::Value Params;
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

class PrefabLibrary
{
public:
	PrefabLibrary();

	void Reset();
	rapidjson::Document& Document();
	const PrefabDefinition* Find(const std::string& _id) const;
	void Add(PrefabDefinition&& _definition);
	std::vector<std::string> GetTexturePaths() const;

private:
	rapidjson::Document PrefabDocument;
	std::unordered_map<std::string, PrefabDefinition> Definitions;
};
