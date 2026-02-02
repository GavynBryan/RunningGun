#include <game/components/ComponentRegistration.h>
#include <core/entity/ComponentRegistryService.h>
#include <core/framework/GameServiceHost.h>
#include <core/entity/Entity.h>
#include <core/util/Json.h>
#include <core/math/Vec2.h>
#include <core/components/RigidBody2D.h>
#include <game/components/BullComponent.h>
#include <game/components/PatrolAIComponent.h>
#include <game/components/PlayerComponent.h>
#include <game/components/ProjectileComponent.h>
#include <game/input/PlayerInputConfig.h>
#include <string>
#include <string_view>

void RegisterDefaultComponents(ComponentRegistryService& registry, const PlayerInputConfig& inputConfig)
{
	registry.Register("player", [&inputConfig](Actor& entity, GameServiceHost& context, std::string_view paramsJson) {
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
	registry.Register("physics", [](Actor& entity, GameServiceHost& context, std::string_view paramsJson) {
		float gravityScale = 1.0f;
		float width = entity.GetWidth();
		float height = entity.GetHeight();
		if (!paramsJson.empty()) {
			auto result = Json::Parse(std::string(paramsJson));
			if (!result.error()) {
				gravityScale = static_cast<float>(Json::GetDouble(result.value(), "gravityScale", gravityScale));
				width = static_cast<float>(Json::GetDouble(result.value(), "width", width));
				height = static_cast<float>(Json::GetDouble(result.value(), "height", height));
			}
		}
		auto component = std::make_unique<RigidBody2DComponent>(entity, context, Vec2(width, height));
		component->SetGravityScale(gravityScale);
		return component;
	});
	registry.Register("patrol_ai", [](Actor& entity, GameServiceHost& context, std::string_view paramsJson) {
		float speed = 150.0f;
		if (!paramsJson.empty()) {
			auto result = Json::Parse(std::string(paramsJson));
			if (!result.error()) {
				speed = Json::GetDouble(result.value(), "speed", speed);
			}
		}
		return std::make_unique<PatrolAIComponent>(entity, context, speed);
	});
	registry.Register("bull", [](Actor& entity, GameServiceHost& context, std::string_view) {
		return std::make_unique<BullComponent>(entity, context);
	});
	registry.Register("projectile", [](Actor& entity, GameServiceHost& context, std::string_view paramsJson) {
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
