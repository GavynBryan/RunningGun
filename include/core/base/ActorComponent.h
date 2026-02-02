#pragma once

#include <simdjson.h>

class Actor;
class GameServiceHost;
class LoggingService;

namespace Json {
	class Writer;
}

class ActorComponent
{
private:
	Actor& Owner;
protected:
	LoggingService* Log = nullptr;
	bool Active = true;

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
