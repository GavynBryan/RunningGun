#pragma once

// Describes a serializable property for component configuration via JSON.
// Used to provide compile-time documentation of what properties each component accepts.
struct PropertyDescriptor
{
	const char* name;         // JSON key name
	const char* type;         // Data type (e.g., "float", "int", "bool")
	const char* defaultValue; // Default value as string
	const char* description;  // Human-readable description
};
