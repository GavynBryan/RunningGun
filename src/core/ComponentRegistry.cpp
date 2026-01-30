#include <core/ComponentRegistry.h>

void ComponentRegistry::Register(std::string_view type, Factory factory)
{
	Factories[std::string(type)] = std::move(factory);
}

const ComponentRegistry::Factory* ComponentRegistry::Find(std::string_view type) const
{
	auto iter = Factories.find(std::string(type));
	if (iter == Factories.end()) {
		return nullptr;
	}
	return &iter->second;
}

bool ComponentRegistry::Contains(std::string_view type) const
{
	return Factories.find(std::string(type)) != Factories.end();
}

size_t ComponentRegistry::Count() const
{
	return Factories.size();
}
