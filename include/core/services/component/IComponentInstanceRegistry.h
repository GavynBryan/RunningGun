#pragma once

#include <core/services/framework/IService.h>
#include <algorithm>
#include <cassert>
#include <span>
#include <unordered_map>
#include <vector>

class ActorComponent;

//=============================================================================
// IComponentInstanceRegistry
// 
// Interface for component instance registries. Enables type-erased
// registration/unregistration so ActorComponent can hold handles without
// knowing the concrete registry type.
// 
// Implementations that want to be services should also inherit from IService.
//=============================================================================
class IComponentInstanceRegistry
{
public:
	virtual ~IComponentInstanceRegistry() = default;
	virtual void RegisterComponent(ActorComponent* component) = 0;
	virtual void UnregisterComponent(ActorComponent* component) = 0;
	
	// Called when a registered component's state changes in a way that
	// may require the registry to update (e.g., re-sort). Default no-op.
	virtual void MarkDirty() {}
};

//=============================================================================
// RegistryHandle
// 
// RAII handle that automatically unregisters a component from a registry
// when destroyed or reset. Components can hold multiple handles for
// registration with multiple registries.
//=============================================================================
class RegistryHandle
{
public:
	RegistryHandle() = default;
	
	// Registers the component and takes ownership of unregistration
	RegistryHandle(IComponentInstanceRegistry* registry, ActorComponent* component)
		: Registry(registry)
		, Component(component)
	{
		if (Registry && Component)
		{
			Registry->RegisterComponent(Component);
		}
	}

	~RegistryHandle()
	{
		Reset();
	}

	// Move-only semantics
	RegistryHandle(RegistryHandle&& other) noexcept
		: Registry(other.Registry)
		, Component(other.Component)
	{
		other.Registry = nullptr;
		other.Component = nullptr;
	}

	RegistryHandle& operator=(RegistryHandle&& other) noexcept
	{
		if (this != &other)
		{
			Reset();
			Registry = other.Registry;
			Component = other.Component;
			other.Registry = nullptr;
			other.Component = nullptr;
		}
		return *this;
	}

	RegistryHandle(const RegistryHandle&) = delete;
	RegistryHandle& operator=(const RegistryHandle&) = delete;

	// Manually release the handle (unregisters from registry)
	void Reset()
	{
		if (Registry && Component)
		{
			Registry->UnregisterComponent(Component);
		}
		Registry = nullptr;
		Component = nullptr;
	}

	// Check if handle is valid
	bool IsValid() const { return Registry != nullptr && Component != nullptr; }
	explicit operator bool() const { return IsValid(); }

	// Access the registry (for advanced use cases)
	IComponentInstanceRegistry* GetRegistry() const { return Registry; }

private:
	IComponentInstanceRegistry* Registry = nullptr;
	ActorComponent* Component = nullptr;
};

//=============================================================================
// ComponentInstanceRegistry<T>
// 
// Templated service that maintains a registry of all active instances of
// a specific component type. Enables ECS-style iteration patterns where
// systems iterate directly over component pools rather than all entities.
// 
// Features:
//   - O(1) registration and duplicate checking
//   - O(1) unregistration via swap-and-pop
//   - Components self-register if registry service exists
//   - Automatic cleanup via RegistryHandle RAII
//   - Optional dirty flag for lazy operations (sorting, filtering)
// 
// Usage:
//   // In game setup:
//   services.AddService<ComponentInstanceRegistry<RigidBody2DComponent>>();
//   
//   // In component constructor:
//   TryRegisterToInstanceRegistry<ComponentInstanceRegistry<RigidBody2DComponent>>(services);
//   
//   // In system:
//   for (auto* rb : services.Get<ComponentInstanceRegistry<RigidBody2DComponent>>().GetComponents())
//       ApplyGravity(rb);
//=============================================================================
template<typename T>
class ComponentInstanceRegistry : public IComponentInstanceRegistry, public IService
{
	static_assert(std::is_base_of_v<ActorComponent, T>, 
		"ComponentInstanceRegistry<T> requires T to derive from ActorComponent");

public:
	// Type-erased register (called by RegistryHandle constructor)
	void RegisterComponent(ActorComponent* component) override
	{
		Register(static_cast<T*>(component));
	}

	// Type-erased unregister (called by RegistryHandle destructor)
	void UnregisterComponent(ActorComponent* component) override
	{
		Unregister(static_cast<T*>(component));
	}

	// Direct register (O(1) via index map)
	void Register(T* component)
	{
		assert(component != nullptr);
		
		// O(1) duplicate check via index map
		auto it = IndexMap.find(component);
		if (it != IndexMap.end())
		{
			return; // Already registered
		}

		// Add to vector and track index
		size_t index = Components.size();
		Components.push_back(component);
		IndexMap[component] = index;
		IsDirty = true;
	}

	// Direct unregister (O(1) via swap-and-pop)
	void Unregister(T* component)
	{
		auto it = IndexMap.find(component);
		if (it == IndexMap.end())
		{
			return; // Not registered
		}

		size_t indexToRemove = it->second;
		size_t lastIndex = Components.size() - 1;

		// Swap with last element if not already last
		if (indexToRemove != lastIndex)
		{
			T* lastComponent = Components[lastIndex];
			Components[indexToRemove] = lastComponent;
			IndexMap[lastComponent] = indexToRemove;
		}

		// Remove last element
		Components.pop_back();
		IndexMap.erase(it);
		IsDirty = true;
	}

	// Get all registered components (read-only span for safe iteration)
	std::span<T* const> GetComponents() const
	{
		return std::span<T* const>(Components.data(), Components.size());
	}

	// Get mutable access to components (use with care)
	std::span<T*> GetComponentsMutable()
	{
		return std::span<T*>(Components.data(), Components.size());
	}

	// Check if a component is registered
	bool Contains(T* component) const
	{
		return IndexMap.find(component) != IndexMap.end();
	}

	// Get number of registered components
	size_t Count() const { return Components.size(); }

	// Check if empty
	bool IsEmpty() const { return Components.empty(); }

	// Mark registry as dirty (for use with lazy operations like sorting)
	void MarkDirty() { IsDirty = true; }

	// Check and clear dirty flag
	bool CheckAndClearDirty()
	{
		bool wasDirty = IsDirty;
		IsDirty = false;
		return wasDirty;
	}

	// Sort components using a comparator (only if dirty)
	template<typename Comparator>
	void SortIfDirty(Comparator comp)
	{
		if (!IsDirty) return;

		std::sort(Components.begin(), Components.end(), comp);
		
		// Rebuild index map after sort
		for (size_t i = 0; i < Components.size(); ++i)
		{
			IndexMap[Components[i]] = i;
		}
		
		IsDirty = false;
	}

	// Clear all registrations (does NOT notify components)
	void Clear()
	{
		Components.clear();
		IndexMap.clear();
		IsDirty = false;
	}

	// Range-based for loop support
	auto begin() const { return Components.begin(); }
	auto end() const { return Components.end(); }

private:
	std::vector<T*> Components;
	std::unordered_map<T*, size_t> IndexMap;  // Component -> index for O(1) lookup/removal
	bool IsDirty = false;
};

