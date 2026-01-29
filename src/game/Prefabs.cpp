#include <core/Prefabs.h>
#include <core/engine/GameContext.h>
#include <core/PrefabComponentRegistry.h>
#include <core/PrefabFactory.h>
#include <core/PrefabLibrary.h>
#include <core/PrefabLoader.h>
#include <game/components/BullComponent.h>
#include <game/components/PatrolAIComponent.h>
#include <game/components/PhysicsComponent.h>
#include <game/components/PlayerComponent.h>
#include <cassert>

namespace {
	PrefabLibrary PrefabLibraryContext;
	PrefabComponentRegistry ComponentRegistry;
}

Prefabs::Prefabs()
{
}

Prefabs::~Prefabs()
{
}

bool Prefabs::LoadDefinitions(GameContext& _context, const std::string& _path)
{
	if (!PrefabLoader::LoadFromFile(_path, PrefabLibraryContext)) {
		return false;
	}

	auto* _handler = _context.GetTextureHandler();
	for (const auto& _texture : PrefabLibraryContext.GetTexturePaths()) {
		_handler->Load(_texture);
	}
	return true;
}

void Prefabs::RegisterDefaultComponents()
{
	ComponentRegistry.Register("player", [](Entity& _entity, GameContext& _context, const rapidjson::Value&) {
		return std::make_unique<PlayerComponent>(_entity, _context);
	});
	ComponentRegistry.Register("physics", [](Entity& _entity, GameContext& _context, const rapidjson::Value&) {
		return std::make_unique<PhysicsComponent>(_entity, _context);
	});
	ComponentRegistry.Register("patrol_ai", [](Entity& _entity, GameContext& _context, const rapidjson::Value& _params) {
		float _speed = 150.0f;
		if (_params.HasMember("speed") && _params["speed"].IsNumber()) {
			_speed = static_cast<float>(_params["speed"].GetDouble());
		}
		return std::make_unique<PatrolAIComponent>(_entity, _context, _speed);
	});
	ComponentRegistry.Register("bull", [](Entity& _entity, GameContext& _context, const rapidjson::Value&) {
		return std::make_unique<BullComponent>(_entity, _context);
	});
}

PrefabComponentRegistry& Prefabs::GetComponentRegistry()
{
	return ComponentRegistry;
}

PrefabLibrary& Prefabs::GetLibrary()
{
	return PrefabLibraryContext;
}

std::unique_ptr<Entity> Prefabs::GetPlayer(GameContext& _context)
{
	const PrefabDefinition* _definition = PrefabLibraryContext.Find("player");
	assert(_definition);
	return PrefabFactory::CreateEntity(_context, *_definition, ComponentRegistry);
}

std::unique_ptr<Entity> Prefabs::GetBull(GameContext& _context)
{
	const PrefabDefinition* _definition = PrefabLibraryContext.Find("bull");
	assert(_definition);
	return PrefabFactory::CreateEntity(_context, *_definition, ComponentRegistry);
}

std::unique_ptr<Entity> Prefabs::GetScorpion(GameContext& _context)
{
	const PrefabDefinition* _definition = PrefabLibraryContext.Find("scorpion");
	assert(_definition);
	return PrefabFactory::CreateEntity(_context, *_definition, ComponentRegistry);
}
