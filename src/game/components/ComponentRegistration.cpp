#include <game/components/ComponentRegistration.h>
#include <core/ComponentRegistry.h>
#include <core/engine/GameServiceHost.h>
#include <core/Entity.h>
#include <core/Json.h>
#include <game/components/BullComponent.h>
#include <game/components/PatrolAIComponent.h>
#include <game/components/PhysicsComponent.h>
#include <game/components/PlayerComponent.h>
#include <game/components/ProjectileComponent.h>
#include <game/input/PlayerInputConfig.h>
#include <string>
#include <string_view>

void RegisterDefaultComponents(ComponentRegistry& registry, const PlayerInputConfig& inputConfig)
{
	registry.Register("player", [&inputConfig](Entity& entity, GameServiceHost& context, std::string_view paramsJson) {
		float groundAcceleration = 2000.0f;
		float groundDeceleration = 3500.0f;
		if (!paramsJson.empty()) {
			auto result = Json::Parse(std::string(paramsJson));
			if (!result.error()) {
				groundAcceleration = static_cast<float>(Json::GetDouble(result.value(), "groundAcceleration", groundAcceleration));
				groundDeceleration = static_cast<float>(Json::GetDouble(result.value(), "groundDeceleration", groundDeceleration));
			}
		}
		auto component = std::make_unique<PlayerComponent>(entity, context, inputConfig);
		component->SetGroundAcceleration(groundAcceleration);
		component->SetGroundDeceleration(groundDeceleration);
		return component;
	});
	registry.Register("physics", [](Entity& entity, GameServiceHost& context, std::string_view paramsJson) {
		float gravityScale = 1.0f;
		if (!paramsJson.empty()) {
			auto result = Json::Parse(std::string(paramsJson));
			if (!result.error()) {
				gravityScale = static_cast<float>(Json::GetDouble(result.value(), "gravityScale", gravityScale));
			}
		}
		auto component = std::make_unique<PhysicsComponent>(entity, context);
		component->SetGravityScale(gravityScale);
		return component;
	});
	registry.Register("patrol_ai", [](Entity& entity, GameServiceHost& context, std::string_view paramsJson) {
		float speed = 150.0f;
		if (!paramsJson.empty()) {
			auto result = Json::Parse(std::string(paramsJson));
			if (!result.error()) {
				speed = Json::GetDouble(result.value(), "speed", speed);
			}
		}
		return std::make_unique<PatrolAIComponent>(entity, context, speed);
	});
	registry.Register("bull", [](Entity& entity, GameServiceHost& context, std::string_view) {
		return std::make_unique<BullComponent>(entity, context);
	});
	registry.Register("projectile", [](Entity& entity, GameServiceHost& context, std::string_view paramsJson) {
		float speed = 400.0f;
		float lifeSpan = 3.0f;
		if (!paramsJson.empty()) {
			auto result = Json::Parse(std::string(paramsJson));
			if (!result.error()) {
				speed = Json::GetDouble(result.value(), "speed", speed);
				lifeSpan = Json::GetDouble(result.value(), "lifeSpan", lifeSpan);
			}
		}
		return std::make_unique<ProjectileComponent>(entity, context, speed, lifeSpan);
	});
}
