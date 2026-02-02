#pragma once

#include <core/math/Rect.h>
#include <cstdint>

using ProxyHandle = uint32_t;
constexpr ProxyHandle InvalidProxyHandle = 0;

// Default collision layer constants
namespace CollisionLayer
{
	constexpr uint32_t None    = 0;
	constexpr uint32_t Default = 1 << 0;
	constexpr uint32_t Player  = 1 << 1;
	constexpr uint32_t Enemy   = 1 << 2;
	constexpr uint32_t Projectile = 1 << 3;
	constexpr uint32_t Environment = 1 << 4;
	constexpr uint32_t Trigger = 1 << 5;
	constexpr uint32_t All     = 0xFFFFFFFF;
}

// Lightweight spatial representation for QuadTree queries.
// Decoupled from physics - any system can create proxies for spatial partitioning.
struct QuadTreeProxy
{
	ProxyHandle Handle = InvalidProxyHandle;
	Rectf Bounds;
	Rectf PreviousBounds;  // Cached for dirty detection
	bool Enabled = true;
	bool IsTrigger = false;  // Triggers detect overlaps without physics response
	bool Dirty = true;       // Marked when bounds change, cleared after tree update
	uint32_t LayerMask = CollisionLayer::Default;     // What layer(s) this proxy belongs to
	uint32_t CollisionMask = CollisionLayer::All;     // What layer(s) this proxy can collide with
	void* UserData = nullptr;  // Optional back-reference for systems that need it

	// Helper to check if this proxy can collide with another
	bool CanCollideWith(const QuadTreeProxy& other) const
	{
		return (LayerMask & other.CollisionMask) != 0 
		    && (other.LayerMask & CollisionMask) != 0;
	}

	// Mark proxy as dirty if bounds changed
	void MarkDirtyIfMoved()
	{
		if (Bounds.x != PreviousBounds.x || Bounds.y != PreviousBounds.y ||
		    Bounds.width != PreviousBounds.width || Bounds.height != PreviousBounds.height) {
			Dirty = true;
			PreviousBounds = Bounds;
		}
	}
};
