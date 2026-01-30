#include <core/PrefabComponentRegistry.h>

void PrefabComponentRegistry::Register(const std::string& _type, Factory _factory)
{
	Factories[_type] = std::move(_factory);
}

const PrefabComponentRegistry::Factory* PrefabComponentRegistry::Find(const std::string& _type) const
{
	auto _iter = Factories.find(_type);
	if (_iter == Factories.end()) {
		return nullptr;
	}
	return &_iter->second;
}

bool PrefabComponentRegistry::Contains(const std::string& _type) const
{
	return Factories.find(_type) != Factories.end();
}

size_t PrefabComponentRegistry::Count() const
{
	return Factories.size();
}
