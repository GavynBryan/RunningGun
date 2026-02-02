#include <core/services/actor/PrefabService.h>
#include <core/services/framework/GameServiceHost.h>
#include <core/services/component/ComponentRegistryService.h>
#include <core/services/rendering/TextureService.h>
#include <core/services/animation/AnimationClipLibrary.h>
#include <core/base/Actor.h>
#include <core/components/SpriteComponent.h>
#include <core/components/AnimatorComponent.h>
#include <core/util/Json.h>
#include <fstream>

bool PrefabService::LoadFromFile(const std::string& path)
{
	auto result = Json::ParseFile(path);
	if (result.error()) {
		return false;
	}

	simdjson::dom::element doc = result.value();
	auto prefabs = doc["prefabs"].get_array();
	if (prefabs.error()) {
		return false;
	}

	for (auto prefab : prefabs.value()) {
		PrefabDefinition def = ParsePrefabDefinition(prefab);
		Definitions[def.Id] = std::move(def);
	}
	
	return true;
}

PrefabDefinition PrefabService::ParsePrefabDefinition(simdjson::dom::element prefab) const
{
	PrefabDefinition def;
	def.Id = std::string(Json::GetString(prefab, "id"));
	def.Texture = std::string(Json::GetString(prefab, "texture"));
	def.Width = static_cast<float>(Json::GetDouble(prefab, "width", 32.0));
	def.Height = static_cast<float>(Json::GetDouble(prefab, "height", 32.0));
	
	// World position (used for root actors)
	def.Position = Json::GetVec2(prefab, "position", Vec2(0, 0));
	
	// Local position offset (used for child actors)
	auto localPosResult = prefab["localPosition"].get_array();
	if (!localPosResult.error()) {
		def.LocalPosition = Json::GetVec2(prefab, "localPosition", Vec2(0, 0));
		def.UseLocalPosition = true;
	}
	
	// Parent reference (for linking to existing prefab)
	def.ParentId = std::string(Json::GetString(prefab, "parent", ""));
	
	// Tag
	auto tagStr = Json::GetString(prefab, "tag", "player");
	if (tagStr == "player") def.Tag = player;
	else if (tagStr == "bullet") def.Tag = bullet;
	else if (tagStr == "enemy_bullet") def.Tag = enemy_bullet;
	else if (tagStr == "hazard") def.Tag = hazard;
	else if (tagStr == "pickup") def.Tag = pickup;
	
	// Animation set reference (for shared animations)
	def.AnimationSet = std::string(Json::GetString(prefab, "animationSet", ""));
	
	// Inline animations
	auto animsResult = prefab["animations"].get_array();
	if (!animsResult.error()) {
		for (auto anim : animsResult.value()) {
			AnimationDefinition animDef;
			animDef.Name = std::string(Json::GetString(anim, "name"));
			animDef.Index = static_cast<int>(Json::GetInt(anim, "index", 0));
			animDef.FrameSize.x = static_cast<float>(Json::GetDouble(anim, "frameWidth", def.Width));
			animDef.FrameSize.y = static_cast<float>(Json::GetDouble(anim, "frameHeight", def.Height));
			animDef.Frames = static_cast<int>(Json::GetInt(anim, "frames", 1));
			animDef.Loop = Json::GetBool(anim, "loop", false);
			animDef.Priority = Json::GetBool(anim, "priority", false);
			def.Animations.push_back(std::move(animDef));
		}
	}
	
	// Components
	auto compsResult = prefab["components"].get_array();
	if (!compsResult.error()) {
		for (auto comp : compsResult.value()) {
			ComponentDefinition compDef;
			compDef.Type = std::string(Json::GetString(comp, "type"));
			
			// Store params as JSON string for factory
			auto paramsResult = comp["params"].get_object();
			if (!paramsResult.error()) {
				compDef.ParamsJson = simdjson::minify(paramsResult.value());
			}
			def.Components.push_back(std::move(compDef));
		}
	}
	
	// Inline children (recursive hierarchy)
	auto childrenResult = prefab["children"].get_array();
	if (!childrenResult.error()) {
		for (auto childPrefab : childrenResult.value()) {
			def.Children.push_back(ParsePrefabDefinition(childPrefab));
		}
	}
	
	return def;
}

bool PrefabService::LoadFromFile(const std::string& path, TextureService& textures)
{
	if (!LoadFromFile(path)) {
		return false;
	}
	
	// Load all referenced textures
	for (const auto& [id, def] : Definitions) {
		if (!def.Texture.empty() && !textures.IsLoaded(def.Texture)) {
			textures.Load(def.Texture);
		}
	}
	
	return true;
}

const PrefabDefinition* PrefabService::Find(std::string_view id) const
{
	auto it = Definitions.find(std::string(id));
	return (it != Definitions.end()) ? &it->second : nullptr;
}

std::unique_ptr<Actor> PrefabService::Instantiate(std::string_view id) const
{
	const PrefabDefinition* def = Find(id);
	if (!def) {
		return nullptr;
	}
	return Instantiate(*def);
}

std::unique_ptr<Actor> PrefabService::Instantiate(const PrefabDefinition& definition) const
{
	auto actor = std::make_unique<Actor>();
	actor->SetTag(definition.Tag);
	
	// Set position based on whether this is a child or root actor
	if (definition.UseLocalPosition) {
		actor->SetLocalPosition(definition.LocalPosition);
	} else {
		actor->SetPosition(definition.Position);
	}
	
	// Note: Components need to be created by the caller using ComponentRegistryService
	// since they require GameServiceHost which we don't have access to here.
	// The caller should use the component definitions from the prefab.
	
	return actor;
}

std::unique_ptr<Actor> PrefabService::InstantiateWithChildren(
	std::string_view id,
	std::vector<std::unique_ptr<Actor>>* outChildren) const
{
	const PrefabDefinition* def = Find(id);
	if (!def) {
		return nullptr;
	}
	return InstantiateWithChildren(*def, outChildren);
}

std::unique_ptr<Actor> PrefabService::InstantiateWithChildren(
	const PrefabDefinition& definition,
	std::vector<std::unique_ptr<Actor>>* outChildren) const
{
	auto root = Instantiate(definition);
	if (!root) {
		return nullptr;
	}

	// Instantiate inline children recursively
	InstantiateChildrenRecursive(root.get(), definition.Children, outChildren);

	return root;
}

void PrefabService::InstantiateChildrenRecursive(
	Actor* parent,
	const std::vector<PrefabDefinition>& children,
	std::vector<std::unique_ptr<Actor>>* outChildren) const
{
	for (const auto& childDef : children) {
		auto child = Instantiate(childDef);
		if (!child) {
			continue;
		}

		Actor* childPtr = child.get();
		
		// Set parent relationship (preserves local position since we already set it)
		child->SetParent(parent, false);

		// Recursively instantiate grandchildren
		InstantiateChildrenRecursive(childPtr, childDef.Children, outChildren);

		// Add to output list if provided
		if (outChildren) {
			outChildren->push_back(std::move(child));
		}
	}
}

std::vector<std::string> PrefabService::GetTexturePaths() const
{
	std::vector<std::string> paths;
	for (const auto& [id, def] : Definitions) {
		if (!def.Texture.empty()) {
			// Avoid duplicates
			bool found = false;
			for (const auto& path : paths) {
				if (path == def.Texture) {
					found = true;
					break;
				}
			}
			if (!found) {
				paths.push_back(def.Texture);
			}
		}
	}
	return paths;
}

void PrefabService::RegisterAnimationSets(AnimationClipLibrary& library) const
{
	for (const auto& [id, def] : Definitions) {
		// Skip prefabs that reference an existing animation set
		if (!def.AnimationSet.empty()) {
			continue;
		}
		
		// Skip prefabs with no inline animations
		if (def.Animations.empty()) {
			continue;
		}
		
		// Create an animation set from inline animations
		AnimationSet set;
		set.Name = id;
		
		for (const auto& animDef : def.Animations) {
			AnimationClipData clip;
			clip.Name = animDef.Name;
			clip.FrameSize = animDef.FrameSize;
			clip.RowIndex = animDef.Index;
			clip.FrameCount = animDef.Frames;
			clip.Looping = animDef.Loop;
			clip.FrameDuration = 0.1f; // Default frame duration
			
			set.AddClip(animDef.Name, std::move(clip));
		}
		
		library.RegisterSet(std::move(set));
	}
}

size_t PrefabService::Count() const
{
	return Definitions.size();
}

void PrefabService::Clear()
{
	Definitions.clear();
}
