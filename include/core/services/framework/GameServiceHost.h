#pragma once

#include <core/containers/BatchArray.h>
#include <core/framework/IService.h>
#include <cassert>
#include <stdexcept>
#include <typeindex>
#include <unordered_map>
#include <vector>
#include <memory>
#include <utility>

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
	// BatchArray Management
	// 
	// Batch arrays can be registered by the type they track.
	// This allows components to find their array without knowing the
	// concrete BatchArray class name.
	// 
	// Usage:
	//     // Setup: add a batch array for IRenderable
	//     services.AddBatchArray<IRenderable>();
	//     
	//     // Component lookup:
	//     auto* array = services.TryGetBatchArray<IRenderable>();
	//=========================================================================

	// Add a BatchArray<T> as a service and register it for type T lookup
	template <typename T>
	BatchArray<T>& AddBatchArray();

	template <typename TComponent>
	void RegisterBatchArray(IBatchArray& batchArray);

	template <typename TComponent>
	IBatchArray* TryGetBatchArray() const;

	template <typename TComponent>
	bool HasBatchArray() const;

private:
	std::vector<std::unique_ptr<IService>> Services;
	std::unordered_map<std::type_index, IService*> Registry;
	std::unordered_map<std::type_index, IBatchArray*> BatchArrays;
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

template <typename T>
BatchArray<T>& GameServiceHost::AddBatchArray()
{
	auto& batchArray = AddService<BatchArray<T>>();
	RegisterBatchArray<T>(batchArray);
	return batchArray;
}

template <typename TComponent>
void GameServiceHost::RegisterBatchArray(IBatchArray& batchArray)
{
	auto typeIndex = std::type_index(typeid(TComponent));
	assert(BatchArrays.find(typeIndex) == BatchArrays.end());
	BatchArrays.emplace(typeIndex, &batchArray);
}

template <typename TComponent>
IBatchArray* GameServiceHost::TryGetBatchArray() const
{
	auto iter = BatchArrays.find(std::type_index(typeid(TComponent)));
	if (iter == BatchArrays.end()) {
		return nullptr;
	}
	return iter->second;
}

template <typename TComponent>
bool GameServiceHost::HasBatchArray() const
{
	return BatchArrays.find(std::type_index(typeid(TComponent))) != BatchArrays.end();
}
