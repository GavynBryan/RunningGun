#pragma once

#include <core/engine/IService.h>
#include <cassert>
#include <typeindex>
#include <unordered_map>
#include <vector>
#include <memory>
#include <utility>

class GameServiceHost
{
public:
	template <typename T, typename... Args>
	T& AddService(int order, Args&&... args);

	template <typename T>
	T& Get() const;

	void Init();
	void Update();
	void Shutdown();

private:
	struct ServiceEntry
	{
		int Order = 0;
		std::unique_ptr<IService> Service;
	};

	std::vector<ServiceEntry> Services;
	std::unordered_map<std::type_index, IService*> Registry;
	bool Initialized = false;

	void SortServices();
};

template <typename T, typename... Args>
T& GameServiceHost::AddService(int order, Args&&... args)
{
	auto service = std::make_unique<T>(std::forward<Args>(args)...);
	service->SetHost(this);
	auto* rawService = service.get();

	auto typeIndex = std::type_index(typeid(T));
	assert(Registry.find(typeIndex) == Registry.end());
	Registry.emplace(typeIndex, rawService);
	Services.push_back({order, std::move(service)});

	if (Initialized) {
		SortServices();
		rawService->Init();
	}

	return *static_cast<T*>(rawService);
}

template <typename T>
T& GameServiceHost::Get() const
{
	auto iter = Registry.find(std::type_index(typeid(T)));
	assert(iter != Registry.end());
	return *static_cast<T*>(iter->second);
}
