#pragma once

#include <core/collision/QuadTreeProxy.h>
#include <core/math/Rect.h>
#include <core/framework/IService.h>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <memory>
#include <utility>

// Represents a collision pair (always ordered so A < B)
struct CollisionPair
{
	ProxyHandle A;
	ProxyHandle B;
	bool IsTriggerCollision = false;  // True if either proxy is a trigger

	bool operator==(const CollisionPair& other) const
	{
		return A == other.A && B == other.B;
	}
};

// Hash function for CollisionPair
struct CollisionPairHash
{
	size_t operator()(const CollisionPair& pair) const
	{
		return std::hash<uint64_t>()(
			(static_cast<uint64_t>(pair.A) << 32) | static_cast<uint64_t>(pair.B)
		);
	}
};

using CollisionSet = std::unordered_set<CollisionPair, CollisionPairHash>;

// Configuration for creating a collision proxy
struct ProxyOptions
{
	Rectf Bounds;
	bool Enabled = true;
	bool IsTrigger = false;
	uint32_t LayerMask = CollisionLayer::Default;
	uint32_t CollisionMask = CollisionLayer::All;
	void* UserData = nullptr;
};

// Owns all collision proxies and stores collision state across frames.
// Any component needing collision (RigidBody2D, triggers, tiles) creates a proxy here.
// QuadTreeSystem reads proxies and writes overlaps; CollisionResponseSystem reads overlaps.
class CollisionService final : public IService
{
public:
	CollisionService() = default;

	// ===== Proxy Management =====
	// Create a new proxy with full options
	ProxyHandle CreateProxy(const ProxyOptions& options);
	
	// Convenience overload for simple cases
	ProxyHandle CreateProxy(const Rectf& bounds, bool enabled = true, void* userData = nullptr);
	
	// Destroy a proxy by handle
	void DestroyProxy(ProxyHandle handle);
	
	// Get proxy by handle (returns nullptr if not found)
	QuadTreeProxy* GetProxy(ProxyHandle handle);
	const QuadTreeProxy* GetProxy(ProxyHandle handle) const;
	
	// Type-safe user data access
	template<typename T>
	T* GetUserData(ProxyHandle handle) const
	{
		if (const auto* proxy = GetProxy(handle)) {
			return static_cast<T*>(proxy->UserData);
		}
		return nullptr;
	}

	// Get all active proxies for QuadTreeCollisionSystem
	const std::vector<QuadTreeProxy*>& GetAllProxies() const { return ActiveProxies; }

	// Get proxies that need tree updates (moved/changed)
	const std::vector<QuadTreeProxy*>& GetDirtyProxies() const { return DirtyProxies; }
	
	// Mark a proxy as dirty (call when bounds change)
	void MarkDirty(ProxyHandle handle);
	
	// Clear dirty flags after tree update
	void ClearDirtyFlags();

	// ===== Overlap Recording =====
	// Called by QuadTreeSystem to record an overlap this frame
	void RecordOverlap(ProxyHandle a, ProxyHandle b, bool isTrigger);

	// Called at end of frame to swap buffers
	void SwapFrames();

	// ===== Collision State Queries =====
	const CollisionSet& GetCurrentOverlaps() const { return CurrentFrame; }
	const CollisionSet& GetPreviousOverlaps() const { return PreviousFrame; }

	bool IsOverlapping(ProxyHandle a, ProxyHandle b) const;
	bool WasOverlapping(ProxyHandle a, ProxyHandle b) const;

	// Get categorized collisions for response system
	void GetCollisionEnters(std::vector<CollisionPair>& outEnters) const;
	void GetCollisionStays(std::vector<CollisionPair>& outStays) const;
	void GetCollisionExits(std::vector<CollisionPair>& outExits) const;

private:
	static CollisionPair MakeOrderedPair(ProxyHandle a, ProxyHandle b);

	// Proxy storage
	ProxyHandle NextHandle = 1;
	std::unordered_map<ProxyHandle, std::unique_ptr<QuadTreeProxy>> Proxies;
	std::vector<QuadTreeProxy*> ActiveProxies;  // Cache for iteration
	std::vector<QuadTreeProxy*> DirtyProxies;   // Proxies needing tree update

	// Collision state
	CollisionSet CurrentFrame;
	CollisionSet PreviousFrame;
};
