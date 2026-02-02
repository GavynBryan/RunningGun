#pragma once

#include <core/services/component/IComponentInstanceRegistry.h>
#include <core/services/framework/GameServiceHost.h>
#include <simdjson.h>
#include <vector>

class Actor;
class LoggingService;

namespace Json {
	class Writer;
}

class ActorComponent
{
private:
	Actor& Owner;
	std::vector<RegistryHandle> RegistryHandles;  // Auto-unregister on destruction

protected:
	LoggingService* Log = nullptr;
	bool Active = true;

	//=========================================================================
	// Instance Registry Registration
	// 
	// Derived components call TryRegisterToInstanceRegistry<T>() in their
	// constructor to opt-in to component instance registries.
	// 
	// The registry is looked up by the component/interface type T (e.g.,
	// IRenderable, RigidBody2DComponent), not the registry class name.
	// 
	// The RegistryHandle constructor calls RegisterComponent() on the registry.
	// Unregistration is AUTOMATIC: when the component is destroyed, each
	// handle's destructor calls UnregisterComponent() on its registry.
	// 
	// Usage:
	//     TryRegisterToInstanceRegistry<IRenderable>(services);
	//     TryRegisterToInstanceRegistry<RigidBody2DComponent>(services);
	//=========================================================================

	// Register with the instance registry for component type T
	// Returns the registry pointer (for caching), or nullptr if not found
	template<typename T>
	IContiguousPool* TryRegisterToInstanceRegistry(GameServiceHost& services)
	{
		auto* registry = services.TryGetRegistry<T>();
		if (registry)
		{
			RegistryHandles.emplace_back(registry, this);
		}
		return registry;
	}

public:
	ActorComponent(Actor& owner, GameServiceHost& services);
	virtual ~ActorComponent();

	// Component identity - used for logging, serialization, debugging
	virtual const char* GetName() const { return "Component"; }
	
	// Type name for serialization (overridden by COMPONENT macro)
	virtual const char* GetTypeName() const { return ""; }

	// Assign owner entity (called when component is attached to entity)
	Actor* GetOwner() { return &Owner; }
	const Actor* GetOwner() const { return &Owner; }

	// Lifecycle methods
	virtual void Start();
	virtual void Update();
	virtual void OnCollide(Actor& other);

	// Active state
	void SetActive(bool active) { Active = active; }
	bool IsActive() const { return Active; }

	// Serialization - overridden by COMPONENT macro
	virtual void SerializeProperties(Json::Writer& writer) const;
	virtual void DeserializeProperties(const simdjson::dom::element& json);
};
