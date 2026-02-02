#include <core/services/component/ComponentRegistryService.h>

void ComponentRegistryService::Register(std::string_view type, Factory factory)
{
	Factories[std::string(type)] = std::move(factory);
}

const ComponentRegistryService::Factory* ComponentRegistryService::Find(std::string_view type) const
{
	auto it = Factories.find(std::string(type));
	return (it != Factories.end()) ? &it->second : nullptr;
}

bool ComponentRegistryService::Contains(std::string_view type) const
{
	return Factories.find(std::string(type)) != Factories.end();
}

size_t ComponentRegistryService::Count() const
{
	return Factories.size();
}
