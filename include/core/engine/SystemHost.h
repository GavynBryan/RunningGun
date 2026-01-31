#pragma once

#include <algorithm>
#include <any>
#include <cassert>
#include <functional>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>

class GameServiceHost;

class SystemHost
{
public:
	explicit SystemHost(GameServiceHost& services);

	template <typename T, typename... Args>
	T& AddSystem(int order, Args&&... args);

	template <typename T>
	T& Get() const;

	template <typename T>
	T* TryGet() const;

	template <typename T>
	bool Has() const;

	void Init();
	void Update();
	void Shutdown();

private:
	struct SystemEntry
	{
		int Order = 0;
		std::shared_ptr<void> System;
		void* RawPtr = nullptr;
		std::function<void(void*)> InitFn;
		std::function<void(void*)> UpdateFn;
		std::function<void(void*)> ShutdownFn;
	};

	GameServiceHost& Services;
	std::vector<SystemEntry> Systems;
	std::unordered_map<std::type_index, void*> Registry;
	bool Initialized = false;
	bool Sorted = false;

	void SortSystems();
};

inline SystemHost::SystemHost(GameServiceHost& services)
	: Services(services)
{
}

template <typename T, typename... Args>
T& SystemHost::AddSystem(int order, Args&&... args)
{
	auto typeIndex = std::type_index(typeid(T));
	assert(Registry.find(typeIndex) == Registry.end());

	auto system = std::make_shared<T>(Services, std::forward<Args>(args)...);
	T* rawPtr = system.get();

	SystemEntry entry;
	entry.Order = order;
	entry.System = system;
	entry.RawPtr = rawPtr;

	// Detect Init method
	if constexpr (requires(T& t) { t.Init(); }) {
		entry.InitFn = [](void* ptr) { static_cast<T*>(ptr)->Init(); };
	}

	// Detect Update method
	if constexpr (requires(T& t) { t.Update(); }) {
		entry.UpdateFn = [](void* ptr) { static_cast<T*>(ptr)->Update(); };
	}

	// Detect Shutdown method
	if constexpr (requires(T& t) { t.Shutdown(); }) {
		entry.ShutdownFn = [](void* ptr) { static_cast<T*>(ptr)->Shutdown(); };
	}

	Registry.emplace(typeIndex, rawPtr);
	Systems.push_back(std::move(entry));
	Sorted = false;

	if (Initialized) {
		SortSystems();
		if (Systems.back().InitFn) {
			Systems.back().InitFn(rawPtr);
		}
	}

	return *rawPtr;
}

template <typename T>
T& SystemHost::Get() const
{
	auto iter = Registry.find(std::type_index(typeid(T)));
	assert(iter != Registry.end());
	return *static_cast<T*>(iter->second);
}

template <typename T>
T* SystemHost::TryGet() const
{
	auto iter = Registry.find(std::type_index(typeid(T)));
	if (iter == Registry.end()) {
		return nullptr;
	}
	return static_cast<T*>(iter->second);
}

template <typename T>
bool SystemHost::Has() const
{
	return Registry.find(std::type_index(typeid(T))) != Registry.end();
}

inline void SystemHost::SortSystems()
{
	if (Sorted) {
		return;
	}
	std::stable_sort(Systems.begin(), Systems.end(),
		[](const SystemEntry& lhs, const SystemEntry& rhs) {
			return lhs.Order < rhs.Order;
		});
	Sorted = true;
}

inline void SystemHost::Init()
{
	if (Initialized) {
		return;
	}
	SortSystems();
	for (const auto& entry : Systems) {
		if (entry.InitFn) {
			entry.InitFn(entry.RawPtr);
		}
	}
	Initialized = true;
}

inline void SystemHost::Update()
{
	for (const auto& entry : Systems) {
		if (entry.UpdateFn) {
			entry.UpdateFn(entry.RawPtr);
		}
	}
}

inline void SystemHost::Shutdown()
{
	if (!Initialized) {
		return;
	}
	for (auto it = Systems.rbegin(); it != Systems.rend(); ++it) {
		if (it->ShutdownFn) {
			it->ShutdownFn(it->RawPtr);
		}
	}
	Initialized = false;
}
