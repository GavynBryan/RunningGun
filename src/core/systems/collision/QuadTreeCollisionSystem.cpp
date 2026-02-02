#include <core/systems/collision/QuadTreeCollisionSystem.h>
#include <core/services/framework/GameServiceHost.h>
#include <core/services/collision/CollisionService.h>
#include <core/services/WorldService.h>

QuadTreeCollisionSystem::QuadTreeCollisionSystem(GameServiceHost& services)
	: Services(services)
	, Collisions(services.Get<CollisionService>())
	, World(services.Get<WorldService>())
	, CollisionTree(World.GetWorldBounds())
{
	LastWorldBounds = World.GetWorldBounds();
}

void QuadTreeCollisionSystem::Update()
{
	// Check if world bounds changed
	const Rectf& currentBounds = World.GetWorldBounds();
	if (currentBounds.x != LastWorldBounds.x || 
	    currentBounds.y != LastWorldBounds.y ||
	    currentBounds.width != LastWorldBounds.width || 
	    currentBounds.height != LastWorldBounds.height) {
		NeedsFullRebuild = true;
		LastWorldBounds = currentBounds;
	}

	// Swap collision frames before detecting new overlaps
	Collisions.SwapFrames();

	// Build/update the tree
	if (NeedsFullRebuild) {
		FullRebuild();
		NeedsFullRebuild = false;
	} else {
		IncrementalUpdate();
	}

	// Detect and record overlaps
	DetectOverlaps();

	// Clear dirty flags for next frame
	Collisions.ClearDirtyFlags();
}

void QuadTreeCollisionSystem::FullRebuild()
{
	CollisionTree = QuadTree(World.GetWorldBounds());
	
	for (auto* proxy : Collisions.GetAllProxies()) {
		if (proxy->Enabled) {
			CollisionTree.Insert(proxy);
		}
	}
}

void QuadTreeCollisionSystem::IncrementalUpdate()
{
	// Update only dirty proxies
	for (auto* proxy : Collisions.GetDirtyProxies()) {
		CollisionTree.Update(proxy);
	}
}

void QuadTreeCollisionSystem::DetectOverlaps()
{
	const auto& proxies = Collisions.GetAllProxies();
	
	for (auto* proxyA : proxies) {
		if (!proxyA->Enabled) {
			continue;
		}

		// Query potential collisions
		QueryResults.clear();
		CollisionTree.Query(proxyA->Bounds, QueryResults);

		for (auto* proxyB : QueryResults) {
			// Skip self and disabled proxies
			if (proxyB == proxyA || !proxyB->Enabled) {
				continue;
			}

			// Skip if we've already processed this pair (A > B means we've seen B,A)
			if (proxyA->Handle > proxyB->Handle) {
				continue;
			}

			// Check layer masks
			if (!proxyA->CanCollideWith(*proxyB)) {
				continue;
			}

			// AABB intersection test
			if (proxyA->Bounds.Intersects(proxyB->Bounds)) {
				bool isTrigger = proxyA->IsTrigger || proxyB->IsTrigger;
				Collisions.RecordOverlap(proxyA->Handle, proxyB->Handle, isTrigger);
			}
		}
	}
}
