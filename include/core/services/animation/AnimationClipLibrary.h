#pragma once
#include <core/animation/AnimationClip.h>
#include <core/framework/IService.h>
#include <string>
#include <string_view>
#include <unordered_map>

// An AnimationSet is a named collection of related animation clips.
// Multiple entities can share the same set if their spritesheets have compatible layouts.
// For example, a "small_enemy" set might define idle/walk/damage clips that work
// for scorpions, spiders, and other similarly-structured sprites.
struct AnimationSet
{
	std::string Name;
	std::unordered_map<std::string, AnimationClipData> Clips;

	// Convenience methods
	void AddClip(const std::string& clipName, AnimationClipData clip);
	const AnimationClipData* FindClip(std::string_view clipName) const;
	bool HasClip(std::string_view clipName) const;
};

// AnimationClipLibrary - Global cache for reusable animation sets.
//
// Provides shared animation data that can be referenced by multiple entities.
// Entities with similar spritesheet layouts can share the same AnimationSet,
// regardless of which prefab they came from.
//
// Example:
//   // Define a reusable animation set
//   AnimationSet smallEnemy;
//   smallEnemy.Name = "small_enemy";
//   smallEnemy.AddClip("idle", idleClip);
//   smallEnemy.AddClip("walk", walkClip);
//   library.RegisterSet(std::move(smallEnemy));
//
//   // Multiple prefabs can reference it
//   // In prefabs.json:
//   // { "id": "scorpion", "animationSet": "small_enemy", ... }
//   // { "id": "spider", "animationSet": "small_enemy", ... }
//
// Thread Safety: Not thread-safe. Intended for use during loading phase only.
//
class AnimationClipLibrary final : public IService
{
public:
	AnimationClipLibrary() = default;
	~AnimationClipLibrary() override = default;

	// Register an animation set
	void RegisterSet(AnimationSet set);
	void RegisterSet(const std::string& setName, AnimationSet set);

	// Find a set by name
	const AnimationSet* FindSet(std::string_view setName) const;
	bool HasSet(std::string_view setName) const;

	// Convenience: find a clip within a set
	const AnimationClipData* FindClip(std::string_view setName, std::string_view clipName) const;

	// Get all clip pointers from a set (for bulk registration with controllers)
	std::vector<std::pair<std::string, const AnimationClipData*>> GetClipsFromSet(std::string_view setName) const;

	// Utility
	size_t SetCount() const { return Sets.size(); }
	void Clear() { Sets.clear(); }

private:
	std::unordered_map<std::string, AnimationSet> Sets;
};
