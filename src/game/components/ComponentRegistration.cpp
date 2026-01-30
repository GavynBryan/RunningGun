#include <game/components/ComponentRegistration.h>
#include <core/engine/GameplayServices.h>
#include <core/Entity.h>
#include <core/Json.h>
#include <core/PrefabComponentRegistry.h>
#include <game/components/BullComponent.h>
#include <game/components/PatrolAIComponent.h>
#include <game/components/PhysicsComponent.h>
#include <game/components/PlayerComponent.h>
#include <game/input/PlayerInputConfig.h>
#include <string>
#include <string_view>

void RegisterDefaultComponents(PrefabComponentRegistry& _registry, const PlayerInputConfig& _inputConfig)
{
	_registry.Register("player", [&_inputConfig](Entity& _entity, GameplayServices& _context, std::string_view) {
		return std::make_unique<PlayerComponent>(_entity, _context, _inputConfig);
	});
	_registry.Register("physics", [](Entity& _entity, GameplayServices& _context, std::string_view) {
		return std::make_unique<PhysicsComponent>(_entity, _context);
	});
	_registry.Register("patrol_ai", [](Entity& _entity, GameplayServices& _context, std::string_view _paramsJson) {
		float _speed = 150.0f;
		if (!_paramsJson.empty()) {
			auto _result = Json::Parse(std::string(_paramsJson));
			if (!_result.error()) {
				_speed = Json::GetDouble(_result.value(), "speed", _speed);
			}
		}
		return std::make_unique<PatrolAIComponent>(_entity, _context, _speed);
	});
	_registry.Register("bull", [](Entity& _entity, GameplayServices& _context, std::string_view) {
		return std::make_unique<BullComponent>(_entity, _context);
	});
}
