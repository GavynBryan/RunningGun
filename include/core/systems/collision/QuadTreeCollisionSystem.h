#pragma once

#include <core/framework/ISystem.h>
#include <core/collision/QuadTree.h>
#include <core/collision/QuadTreeProxy.h>
#include <core/math/Rect.h>
#include <vector>

class GameServiceHost;
class CollisionService;
class WorldService;

// Builds QuadTree from registered proxies and detects overlaps.
// Writes collision pairs to CollisionService for response systems to handle.
// Uses incremental updates for moved proxies when possible.
class QuadTreeCollisionSystem final : public ISystem
{
public:
	QuadTreeCollisionSystem(GameServiceHost& services);

	void Update() override;

	// Force a full tree rebuild (e.g., after world bounds change)
	void ForceFullRebuild() { NeedsFullRebuild = true; }

private:
	void FullRebuild();
	void IncrementalUpdate();
	void DetectOverlaps();

	GameServiceHost& Services;
	CollisionService& Collisions;
	WorldService& World;

	QuadTree CollisionTree;
	std::vector<QuadTreeProxy*> QueryResults;
	
	bool NeedsFullRebuild = true;  // First frame needs full build
	Rectf LastWorldBounds;         // Detect world bounds changes
};
