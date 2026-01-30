#include <game/components/ComponentRegistration.h>
#include <core/ComponentRegistry.h>
#include <core/engine/GameplayServices.h>
#include <core/Entity.h>
#include <core/Json.h>
#include <game/components/BullComponent.h>
#include <game/components/PatrolAIComponent.h>
#include <game/components/PhysicsComponent.h>
#include <game/components/PlayerComponent.h>
#include <game/input/PlayerInputConfig.h>
#include <string>
#include <string_view>

void RegisterDefaultComponents(ComponentRegistry& registry, const PlayerInputConfig& inputConfig)
{
	registry.Register("player", [&inputConfig](Entity& entity, GameplayServices& context, std::string_view) {
		return std::make_unique<PlayerComponent>(entity, context, inputConfig);
	});
	registry.Register("physics", [](Entity& entity, GameplayServices& context, std::string_view) {
		return std::make_unique<PhysicsComponent>(entity, context);
	});
	registry.Register("patrol_ai", [](Entity& entity, GameplayServices& context, std::string_view paramsJson) {
		float speed = 150.0f;
		if (!paramsJson.empty()) {
			auto result = Json::Parse(std::string(paramsJson));
			if (!result.error()) {
				speed = Json::GetDouble(result.value(), "speed", speed);
			}
		}
		return std::make_unique<PatrolAIComponent>(entity, context, speed);
	});
	registry.Register("bull", [](Entity& entity, GameplayServices& context, std::string_view) {
		return std::make_unique<BullComponent>(entity, context);
	});
}
