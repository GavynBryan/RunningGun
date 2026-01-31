#pragma once

#include <any>
#include <cassert>
#include <stdexcept>
#include <typeindex>
#include <unordered_map>
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

private:
	struct ServiceEntry
	{
		std::any Service;
		void* RawPtr = nullptr;
	};

	std::unordered_map<std::type_index, ServiceEntry> Registry;
};

template <typename T, typename... Args>
T& GameServiceHost::AddService(Args&&... args)
{
	auto typeIndex = std::type_index(typeid(T));
	assert(Registry.find(typeIndex) == Registry.end());

	auto service = std::make_shared<T>(std::forward<Args>(args)...);
	T* rawPtr = service.get();

	ServiceEntry entry;
	entry.Service = std::move(service);
	entry.RawPtr = rawPtr;

	Registry.emplace(typeIndex, std::move(entry));

	return *rawPtr;
}

template <typename T>
T& GameServiceHost::Get() const
{
	auto iter = Registry.find(std::type_index(typeid(T)));
	if (iter == Registry.end()) {
		throw std::runtime_error("Service not registered: " + std::string(typeid(T).name()));
	}
	return *static_cast<T*>(iter->second.RawPtr);
}

template <typename T>
T* GameServiceHost::TryGet() const
{
	auto iter = Registry.find(std::type_index(typeid(T)));
	if (iter == Registry.end()) {
		return nullptr;
	}
	return static_cast<T*>(iter->second.RawPtr);
}

template <typename T>
bool GameServiceHost::Has() const
{
	return Registry.find(std::type_index(typeid(T))) != Registry.end();
}
