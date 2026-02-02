#pragma once

#include <core/containers/BatchArray.h>
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
	std::vector<BatchArrayHandle> BatchArrayHandles;  // Auto-remove on destruction

protected:
	LoggingService* Log = nullptr;
	bool Active = true;

	//=========================================================================
	// BatchArray Registration
	// 
	// Derived components call TryRegisterToBatchArray<T>() in their
	// constructor to opt-in to batch arrays for contiguous iteration.
	// 
	// The array is looked up by the component/interface type T (e.g.,
	// IRenderable, RigidBody2DComponent), not the BatchArray class name.
	// 
	// The BatchArrayHandle constructor calls Add() on the array.
	// Removal is AUTOMATIC: when the component is destroyed, each
	// handle's destructor calls Remove() on its array.
	// 
	// Usage:
	//     TryRegisterToBatchArray<IRenderable>(services);
	//     TryRegisterToBatchArray<RigidBody2DComponent>(services);
	//=========================================================================

	// Register with the batch array for type T
	// Returns the array pointer (for caching), or nullptr if not found
	template<typename T>
	IBatchArray* TryRegisterToBatchArray(GameServiceHost& services)
	{
		auto* batchArray = services.TryGetBatchArray<T>();
		if (batchArray)
		{
			BatchArrayHandles.emplace_back(batchArray, this);
		}
		return batchArray;
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
