#include <core/services/component/ComponentFactory.h>
#include <core/base/ActorComponent.h>
#include <core/serialization/IDeserializer.h>

std::unique_ptr<ActorComponent> ComponentFactoryService::CreateAndDeserialize(
	std::string_view typeName,
	Actor& actor,
	GameServiceHost& services,
	const IDeserializer& deserializer) const
{
	auto component = Create(typeName, actor, services);
	if (component) {
		component->Deserialize(deserializer);
	}
	return component;
}

