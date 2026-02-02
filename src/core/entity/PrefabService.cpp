#include <core/entity/PrefabService.h>
#include <core/services/component/ComponentFactory.h>
#include <core/base/ActorComponent.h>
#include <core/animation/AnimationClip.h>
#include <core/animation/AnimationClipLibrary.h>
#include <core/components/AnimatorComponent.h>
#include <core/framework/GameServiceHost.h>
#include <core/util/Json.h>
#include <core/rendering/TextureService.h>
#include <core/components/TransformComponent.h>
#include <core/components/SpriteComponent.h>
#include <cassert>

namespace {
	bool TryParseVec2(simdjson::dom::element value, Vec2& out)
	{
		if (value.type() != simdjson::dom::element_type::ARRAY) {
			return false;
		}
		auto arr = value.get_array().value();
		if (arr.size() != 2) {
			return false;
		}
		auto it = arr.begin();
		auto x = (*it).get_double();
		++it;
		auto y = (*it).get_double();
		if (x.error() || y.error()) {
			return false;
		}
		out = Vec2(static_cast<float>(x.value()), static_cast<float>(y.value()));
		return true;
	}

	ENTITY_TAG ParseTag(std::string_view value)
	{
		if (value == "player") return player;
		if (value == "bullet") return bullet;
		if (value == "enemy_bullet") return enemy_bullet;
		if (value == "hazard") return hazard;
		if (value == "pickup") return pickup;
		return player;
	}

	void ParseAnimations(simdjson::dom::element prefab, PrefabDefinition& definition)
	{
		auto animations = prefab["animations"].get_array();
		if (animations.error()) {
			return;
		}
		for (auto anim : animations.value()) {
			if (anim.type() != simdjson::dom::element_type::OBJECT) {
				continue;
			}
			AnimationDefinition animDef;

			auto name = anim["name"].get_string();
			if (!name.error()) {
				animDef.Name = std::string(name.value());
			}

			auto index = anim["index"].get_int64();
			if (!index.error()) {
				animDef.Index = static_cast<int>(index.value());
			}

			auto frames = anim["frames"].get_int64();
			if (!frames.error()) {
				animDef.Frames = static_cast<int>(frames.value());
			}

			auto loop = anim["loop"].get_bool();
			if (!loop.error()) {
				animDef.Loop = loop.value();
			}

			auto priority = anim["priority"].get_bool();
			if (!priority.error()) {
				animDef.Priority = priority.value();
			}

			auto frameSize = anim["frameSize"];
			if (!frameSize.error()) {
				TryParseVec2(frameSize.value(), animDef.FrameSize);
			}

			if (!animDef.Name.empty()) {
				definition.Animations.push_back(std::move(animDef));
			}
		}
	}

	void ParseComponents(simdjson::dom::element prefab, PrefabDefinition& definition)
	{
		auto components = prefab["components"].get_array();
		if (components.error()) {
			return;
		}

		for (auto comp : components.value()) {
			if (comp.type() != simdjson::dom::element_type::OBJECT) {
				continue;
			}
			ComponentDefinition compDef;

			auto type = comp["type"].get_string();
			if (!type.error()) {
				compDef.Type = std::string(type.value());
			}

			auto params = comp["params"];
			if (!params.error() && params.value().type() == simdjson::dom::element_type::OBJECT) {
				compDef.ParamsJson = simdjson::to_string(params.value());
			}

			if (!compDef.Type.empty()) {
				definition.Components.push_back(std::move(compDef));
			}
		}
	}
}

bool PrefabService::LoadFromFile(const std::string& path)
{
	auto result = Json::ParseFile(path);
	if (result.error()) {
		return false;
	}

	simdjson::dom::element root = result.value();
	if (root.type() != simdjson::dom::element_type::OBJECT) {
		return false;
	}

	auto prefabs = root["prefabs"].get_array();
	if (prefabs.error()) {
		return false;
	}

	Clear();

	for (auto prefab : prefabs.value()) {
		if (prefab.type() != simdjson::dom::element_type::OBJECT) {
			continue;
		}

		PrefabDefinition definition;

		auto id = prefab["id"].get_string();
		if (!id.error()) {
			definition.Id = std::string(id.value());
		}

		auto texture = prefab["texture"].get_string();
		if (!texture.error()) {
			definition.Texture = std::string(texture.value());
		}

		auto width = prefab["width"].get_double();
		if (!width.error()) {
			definition.Width = static_cast<float>(width.value());
		}

		auto height = prefab["height"].get_double();
		if (!height.error()) {
			definition.Height = static_cast<float>(height.value());
		}

		auto position = prefab["position"];
		if (!position.error()) {
			TryParseVec2(position.value(), definition.Position);
		}

		auto tag = prefab["tag"].get_string();
		if (!tag.error()) {
			definition.Tag = ParseTag(tag.value());
		}

		// Check for animationSet reference first
		auto animationSet = prefab["animationSet"].get_string();
		if (!animationSet.error()) {
			definition.AnimationSet = std::string(animationSet.value());
		}

		// Parse inline animations (used if no animationSet is specified)
		ParseAnimations(prefab, definition);
		ParseComponents(prefab, definition);

		if (!definition.Id.empty()) {
			Definitions.emplace(definition.Id, std::move(definition));
		}
	}

	return true;
}

bool PrefabService::LoadFromFile(const std::string& path, TextureService& textures)
{
	if (!LoadFromFile(path)) {
		return false;
	}

	for (const auto& texturePath : GetTexturePaths()) {
		textures.Load(texturePath);
	}
	return true;
}

const PrefabDefinition* PrefabService::Find(std::string_view id) const
{
	auto iter = Definitions.find(std::string(id));
	if (iter == Definitions.end()) {
		return nullptr;
	}
	return &iter->second;
}

std::unique_ptr<Entity> PrefabService::Instantiate(std::string_view id) const
{
	const PrefabDefinition* definition = Find(id);
	if (!definition) {
		return nullptr;
	}
	return Instantiate(*definition);
}

std::unique_ptr<Entity> PrefabService::Instantiate(const PrefabDefinition& definition) const
{
	GameServiceHost& services = GetHost();

	// Create entity (automatically gets TransformComponent)
	auto entity = std::make_unique<Actor>(services);

	// Create SpriteComponent if we have a texture
	if (!definition.Texture.empty()) {
		auto* textureService = services.TryGet<TextureService>();
		if (textureService) {
			TextureHandle texHandle = textureService->Get(definition.Texture);
			if (!texHandle.IsValid()) {
				// Try loading it
				texHandle = textureService->Load(definition.Texture);
			}
			
			auto sprite = std::make_unique<SpriteComponent>(*entity, services);
			sprite->SetTexture(texHandle);
			sprite->SetSize(definition.Width, definition.Height);
			sprite->SetSourceRect(0, 0, static_cast<int>(definition.Width), static_cast<int>(definition.Height));
			entity->AttachComponent(std::move(sprite));
		}
	}

	// Add animations via AnimatorComponent
	// Priority: 1) Reference shared set, 2) Inline animations from set named after prefab, 3) Create owned clips
	auto* clipLibrary = services.TryGet<AnimationClipLibrary>();
	
	// Determine which animation set to use
	std::string_view setName = definition.AnimationSet.empty() ? definition.Id : definition.AnimationSet;
	bool hasAnimations = !definition.AnimationSet.empty() || !definition.Animations.empty();
	
	if (hasAnimations) {
		auto animatorComp = std::make_unique<AnimatorComponent>(*entity, services);
		AnimationController& controller = animatorComp->GetController();
		
		// Try to get clips from shared library
		if (clipLibrary && clipLibrary->HasSet(setName)) {
			// Use shared clips from library
			auto clips = clipLibrary->GetClipsFromSet(setName);
			for (const auto& [clipName, clipPtr] : clips) {
				controller.AddClipRef(clipName, clipPtr);
			}
		} else if (!definition.Animations.empty()) {
			// No shared set available - create owned clips from inline definitions
			for (const auto& animDef : definition.Animations) {
				AnimationClipData clipData;
				clipData.Name = animDef.Name;
				clipData.RowIndex = animDef.Index;
				clipData.FrameSize = animDef.FrameSize;
				if (clipData.FrameSize.x == 0 && clipData.FrameSize.y == 0) {
					clipData.FrameSize = Vec2(definition.Width, definition.Height);
				}
				clipData.FrameCount = animDef.Frames;
				clipData.Looping = animDef.Loop;
				clipData.FrameDuration = 0.25f;
				
				controller.AddClip(animDef.Name, std::move(clipData));
			}
		}
		
		entity->AttachComponent(std::move(animatorComp));
	}

	// Add user-defined components from prefab using self-registering ComponentFactory
	auto& factory = ComponentFactory::Instance();
	for (const auto& component : definition.Components) {
		std::unique_ptr<ActorComponent> comp;
		
		if (component.ParamsJson.empty()) {
			// No parameters - just create with defaults
			comp = factory.Create(component.Type, *entity, services);
		} else {
			// Parse params and deserialize
			auto parseResult = Json::Parse(component.ParamsJson);
			if (!parseResult.error()) {
				comp = factory.CreateFromJson(component.Type, *entity, services, parseResult.value());
			} else {
				// Parse failed, create with defaults
				comp = factory.Create(component.Type, *entity, services);
			}
		}
		
		if (comp) {
			entity->AttachComponent(std::move(comp));
		}
	}

	entity->SetTag(definition.Tag);
	entity->SetPosition(definition.Position);
	return entity;
}

std::vector<std::string> PrefabService::GetTexturePaths() const
{
	std::vector<std::string> paths;
	paths.reserve(Definitions.size());
	for (const auto& pair : Definitions) {
		paths.push_back(pair.second.Texture);
	}
	return paths;
}

void PrefabService::RegisterAnimationSets(AnimationClipLibrary& library) const
{
	for (const auto& [prefabId, definition] : Definitions) {
		// Skip prefabs that reference an existing set (they don't define new animations)
		if (!definition.AnimationSet.empty()) {
			continue;
		}
		
		// Skip prefabs with no inline animations
		if (definition.Animations.empty()) {
			continue;
		}
		
		// Create a set named after the prefab ID
		AnimationSet set;
		set.Name = prefabId;
		
		for (const auto& animDef : definition.Animations) {
			AnimationClipData clipData;
			clipData.Name = animDef.Name;
			clipData.RowIndex = animDef.Index;
			clipData.FrameSize = animDef.FrameSize;
			if (clipData.FrameSize.x == 0 && clipData.FrameSize.y == 0) {
				clipData.FrameSize = Vec2(definition.Width, definition.Height);
			}
			clipData.FrameCount = animDef.Frames;
			clipData.Looping = animDef.Loop;
			clipData.FrameDuration = 0.25f;
			
			set.AddClip(animDef.Name, std::move(clipData));
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
