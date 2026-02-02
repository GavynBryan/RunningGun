#include <core/services/component/ComponentFactory.h>
#include <core/base/ActorComponent.h>

ComponentFactory& ComponentFactory::Instance()
{
	static ComponentFactory instance;
	return instance;
}

void ComponentFactory::Register(ComponentMeta meta)
{
	std::string key(meta.TypeName);
	m_Registry[key] = std::move(meta);
	m_AllComponentsDirty = true;
}

const ComponentMeta* ComponentFactory::Find(std::string_view typeName) const
{
	auto it = m_Registry.find(std::string(typeName));
	return (it != m_Registry.end()) ? &it->second : nullptr;
}

bool ComponentFactory::Contains(std::string_view typeName) const
{
	return m_Registry.find(std::string(typeName)) != m_Registry.end();
}

std::unique_ptr<ActorComponent> ComponentFactory::Create(
	std::string_view typeName,
	Actor& actor,
	GameServiceHost& services) const
{
	const auto* meta = Find(typeName);
	if (!meta || !meta->Factory) {
		return nullptr;
	}
	return meta->Factory(actor, services);
}

std::unique_ptr<ActorComponent> ComponentFactory::CreateFromJson(
	std::string_view typeName,
	Actor& actor,
	GameServiceHost& services,
	const simdjson::dom::element& json) const
{
	auto component = Create(typeName, actor, services);
	if (component) {
		component->DeserializeProperties(json);
	}
	return component;
}

const std::vector<const ComponentMeta*>& ComponentFactory::All() const
{
	if (m_AllComponentsDirty) {
		m_AllComponents.clear();
		m_AllComponents.reserve(m_Registry.size());
		for (const auto& [name, meta] : m_Registry) {
			m_AllComponents.push_back(&meta);
		}
		m_AllComponentsDirty = false;
	}
	return m_AllComponents;
}
