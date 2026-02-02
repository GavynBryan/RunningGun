#pragma once

#include <core/framework/IService.h>
#include <cassert>
#include <stdexcept>
#include <typeindex>
#include <unordered_map>
#include <vector>
#include <memory>
#include <utility>

class IContiguousPool;

class GameServiceHost
{
public:
	template <typename T, typename... Args>
	T& AddService(Args&&... args);

	template <typename T>
	T& Get() const;

	template <typename T>
	T* TryGet() const;

	template <typename T>
	bool Has() const;

	//=========================================================================
	// Component Instance Registry Management
	// 
	// Registries can be registered by the component/interface type they track.
	// This allows components to find their registry without knowing the
	// concrete registry class name.
	// 
	// Usage:
	//     // Setup: register RenderableRegistry as the registry for IRenderable
	//     services.AddService<RenderableRegistry>();
	//     services.RegisterInstanceRegistry<IRenderable>(services.Get<RenderableRegistry>());
	//     
	//     // Component lookup:
	//     auto* registry = services.TryGetRegistry<IRenderable>();
	//=========================================================================

	template <typename TComponent>
	void RegisterInstanceRegistry(IContiguousPool& registry);

	template <typename TComponent>
	IContiguousPool* TryGetRegistry() const;

	template <typename TComponent>
	bool HasRegistry() const;

private:
	std::vector<std::unique_ptr<IService>> Services;
	std::unordered_map<std::type_index, IService*> Registry;
	std::unordered_map<std::type_index, IContiguousPool*> InstanceRegistries;
};

template <typename T, typename... Args>
T& GameServiceHost::AddService(Args&&... args)
{
	static_assert(std::is_base_of<IService, T>::value, "T must derive from IService");

	auto service = std::make_unique<T>(std::forward<Args>(args)...);
	service->SetHost(this);
	auto* rawService = service.get();

	auto typeIndex = std::type_index(typeid(T));
	assert(Registry.find(typeIndex) == Registry.end());
	Registry.emplace(typeIndex, rawService);
	Services.push_back(std::move(service));

	return *rawService;
}

template <typename T>
T& GameServiceHost::Get() const
{
	static_assert(std::is_base_of<IService, T>::value, "T must derive from IService");
	auto iter = Registry.find(std::type_index(typeid(T)));
	if (iter == Registry.end()) {
		throw std::runtime_error("Service not registered: " + std::string(typeid(T).name()));
	}
	return *static_cast<T*>(iter->second);
}

template <typename T>
T* GameServiceHost::TryGet() const
{
	static_assert(std::is_base_of<IService, T>::value, "T must derive from IService");
	auto iter = Registry.find(std::type_index(typeid(T)));
	if (iter == Registry.end()) {
		return nullptr;
	}
	return static_cast<T*>(iter->second);
}

template <typename T>
bool GameServiceHost::Has() const
{
	static_assert(std::is_base_of<IService, T>::value, "T must derive from IService");
	return Registry.find(std::type_index(typeid(T))) != Registry.end();
}

template <typename TComponent>
void GameServiceHost::RegisterInstanceRegistry(IContiguousPool& registry)
{
	auto typeIndex = std::type_index(typeid(TComponent));
	assert(InstanceRegistries.find(typeIndex) == InstanceRegistries.end());
	InstanceRegistries.emplace(typeIndex, &registry);
}

template <typename TComponent>
IContiguousPool* GameServiceHost::TryGetRegistry() const
{
	auto iter = InstanceRegistries.find(std::type_index(typeid(TComponent)));
	if (iter == InstanceRegistries.end()) {
		return nullptr;
	}
	return iter->second;
}

template <typename TComponent>
bool GameServiceHost::HasRegistry() const
{
	return InstanceRegistries.find(std::type_index(typeid(TComponent))) != InstanceRegistries.end();
}
