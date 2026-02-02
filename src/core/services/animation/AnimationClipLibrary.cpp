#include <core/services/animation/AnimationClipLibrary.h>

// AnimationSet implementation

void AnimationSet::AddClip(const std::string& clipName, AnimationClipData clip)
{
	Clips[clipName] = std::move(clip);
}

const AnimationClipData* AnimationSet::FindClip(std::string_view clipName) const
{
	auto it = Clips.find(std::string(clipName));
	return (it != Clips.end()) ? &it->second : nullptr;
}

bool AnimationSet::HasClip(std::string_view clipName) const
{
	return Clips.find(std::string(clipName)) != Clips.end();
}

// AnimationClipLibrary implementation

void AnimationClipLibrary::RegisterSet(AnimationSet set)
{
	std::string name = set.Name;
	Sets[std::move(name)] = std::move(set);
}

void AnimationClipLibrary::RegisterSet(const std::string& setName, AnimationSet set)
{
	set.Name = setName;
	Sets[setName] = std::move(set);
}

const AnimationSet* AnimationClipLibrary::FindSet(std::string_view setName) const
{
	auto it = Sets.find(std::string(setName));
	return (it != Sets.end()) ? &it->second : nullptr;
}

bool AnimationClipLibrary::HasSet(std::string_view setName) const
{
	return Sets.find(std::string(setName)) != Sets.end();
}

const AnimationClipData* AnimationClipLibrary::FindClip(std::string_view setName, std::string_view clipName) const
{
	if (const AnimationSet* set = FindSet(setName)) {
		return set->FindClip(clipName);
	}
	return nullptr;
}

std::vector<std::pair<std::string, const AnimationClipData*>> AnimationClipLibrary::GetClipsFromSet(std::string_view setName) const
{
	std::vector<std::pair<std::string, const AnimationClipData*>> result;
	
	if (const AnimationSet* set = FindSet(setName)) {
		result.reserve(set->Clips.size());
		for (const auto& [name, clip] : set->Clips) {
			result.emplace_back(name, &clip);
		}
	}
	
	return result;
}
