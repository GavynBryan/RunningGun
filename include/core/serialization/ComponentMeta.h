#pragma once

#include <string_view>
#include <vector>
#include <memory>
#include <functional>
#include "PropertyMeta.h"

class ActorComponent;
class Actor;
class GameServiceHost;

// Component metadata for registration and instantiation
struct ComponentMeta
{
	std::string_view TypeName;                    // Serialization key (e.g., "player", "rigidbody2d")
	std::vector<PropertyMeta> Properties;         // All serializable properties
	
	// Factory creates a default-constructed component
	std::function<std::unique_ptr<ActorComponent>(Actor&, GameServiceHost&)> Factory;
};

