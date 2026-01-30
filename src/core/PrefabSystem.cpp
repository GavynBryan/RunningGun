#include <core/PrefabSystem.h>
#include <core/animation/Animation.h>
#include <core/engine/GameplayServices.h>
#include <core/Json.h>
#include <core/ResourceHandler.h>
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

void PrefabSystem::SetServices(GameplayServices& services)
{
	Services = &services;
}

ComponentRegistry& PrefabSystem::GetRegistry()
{
	return Registry;
}

const ComponentRegistry& PrefabSystem::GetRegistry() const
{
	return Registry;
}

bool PrefabSystem::LoadFromFile(const std::string& path)
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

		ParseAnimations(prefab, definition);
		ParseComponents(prefab, definition);

		if (!definition.Id.empty()) {
			Definitions.emplace(definition.Id, std::move(definition));
		}
	}

	return true;
}

bool PrefabSystem::LoadFromFile(const std::string& path, ResourceHandler& textures)
{
	if (!LoadFromFile(path)) {
		return false;
	}

	for (const auto& texturePath : GetTexturePaths()) {
		textures.Load(texturePath);
	}
	return true;
}

const PrefabDefinition* PrefabSystem::Find(std::string_view id) const
{
	auto iter = Definitions.find(std::string(id));
	if (iter == Definitions.end()) {
		return nullptr;
	}
	return &iter->second;
}

std::unique_ptr<Entity> PrefabSystem::Instantiate(std::string_view id) const
{
	const PrefabDefinition* definition = Find(id);
	if (!definition) {
		return nullptr;
	}
	return Instantiate(*definition);
}

std::unique_ptr<Entity> PrefabSystem::Instantiate(const PrefabDefinition& definition) const
{
	assert(Services && "PrefabSystem::SetServices must be called before Instantiate");

	auto entity = std::make_unique<Entity>(*Services, definition.Texture, definition.Width, definition.Height);

	if (!definition.Animations.empty()) {
		auto anim = std::make_unique<AnimationStateMachine>();
		for (const auto& animDef : definition.Animations) {
			Vec2 frameSize = animDef.FrameSize;
			if (frameSize.x == 0 && frameSize.y == 0) {
				frameSize = Vec2(definition.Width, definition.Height);
			}
			auto animPtr = std::make_unique<Animation>(animDef.Index, frameSize, animDef.Frames, animDef.Loop, animDef.Priority);
			anim->AddAnimation(animDef.Name, std::move(animPtr));
		}
		entity->AssignAnimator(std::move(anim));
	}

	for (const auto& component : definition.Components) {
		const auto* factory = Registry.Find(component.Type);
		if (factory) {
			auto comp = (*factory)(*entity, *Services, component.ParamsJson);
			if (comp) {
				entity->AttachComponent(std::move(comp));
			}
		}
	}

	entity->SetTag(definition.Tag);
	entity->SetPosition(definition.Position);
	return entity;
}

std::vector<std::string> PrefabSystem::GetTexturePaths() const
{
	std::vector<std::string> paths;
	paths.reserve(Definitions.size());
	for (const auto& pair : Definitions) {
		paths.push_back(pair.second.Texture);
	}
	return paths;
}

size_t PrefabSystem::Count() const
{
	return Definitions.size();
}

void PrefabSystem::Clear()
{
	Definitions.clear();
}
