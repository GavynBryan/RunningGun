#include <core/services/collision/CollisionService.h>
#include <algorithm>

ProxyHandle CollisionService::CreateProxy(const ProxyOptions& options)
{
	ProxyHandle handle = NextHandle++;
	
	auto proxy = std::make_unique<QuadTreeProxy>();
	proxy->Handle = handle;
	proxy->Bounds = options.Bounds;
	proxy->PreviousBounds = options.Bounds;
	proxy->Enabled = options.Enabled;
	proxy->IsTrigger = options.IsTrigger;
	proxy->LayerMask = options.LayerMask;
	proxy->CollisionMask = options.CollisionMask;
	proxy->UserData = options.UserData;
	proxy->Dirty = true;
	
	QuadTreeProxy* rawProxy = proxy.get();
	Proxies[handle] = std::move(proxy);
	ActiveProxies.push_back(rawProxy);
	DirtyProxies.push_back(rawProxy);
	
	return handle;
}

ProxyHandle CollisionService::CreateProxy(const Rectf& bounds, bool enabled, void* userData)
{
	ProxyOptions options;
	options.Bounds = bounds;
	options.Enabled = enabled;
	options.UserData = userData;
	return CreateProxy(options);
}

void CollisionService::DestroyProxy(ProxyHandle handle)
{
	auto it = Proxies.find(handle);
	if (it == Proxies.end()) {
		return;
	}
	
	QuadTreeProxy* proxy = it->second.get();
	
	// Remove from active proxies
	ActiveProxies.erase(
		std::remove(ActiveProxies.begin(), ActiveProxies.end(), proxy),
		ActiveProxies.end()
	);
	
	// Remove from dirty proxies
	DirtyProxies.erase(
		std::remove(DirtyProxies.begin(), DirtyProxies.end(), proxy),
		DirtyProxies.end()
	);
	
	// Remove any collisions involving this proxy
	auto removeFromSet = [handle](CollisionSet& set) {
		for (auto it = set.begin(); it != set.end(); ) {
			if (it->A == handle || it->B == handle) {
				it = set.erase(it);
			} else {
				++it;
			}
		}
	};
	removeFromSet(CurrentFrame);
	removeFromSet(PreviousFrame);
	
	Proxies.erase(it);
}

QuadTreeProxy* CollisionService::GetProxy(ProxyHandle handle)
{
	auto it = Proxies.find(handle);
	return (it != Proxies.end()) ? it->second.get() : nullptr;
}

const QuadTreeProxy* CollisionService::GetProxy(ProxyHandle handle) const
{
	auto it = Proxies.find(handle);
	return (it != Proxies.end()) ? it->second.get() : nullptr;
}

void CollisionService::MarkDirty(ProxyHandle handle)
{
	if (auto* proxy = GetProxy(handle)) {
		if (!proxy->Dirty) {
			proxy->Dirty = true;
			DirtyProxies.push_back(proxy);
		}
	}
}

void CollisionService::ClearDirtyFlags()
{
	for (auto* proxy : DirtyProxies) {
		proxy->Dirty = false;
	}
	DirtyProxies.clear();
}

void CollisionService::RecordOverlap(ProxyHandle a, ProxyHandle b, bool isTrigger)
{
	CollisionPair pair = MakeOrderedPair(a, b);
	pair.IsTriggerCollision = isTrigger;
	CurrentFrame.insert(pair);
}

void CollisionService::SwapFrames()
{
	PreviousFrame = std::move(CurrentFrame);
	CurrentFrame.clear();
}

bool CollisionService::IsOverlapping(ProxyHandle a, ProxyHandle b) const
{
	return CurrentFrame.find(MakeOrderedPair(a, b)) != CurrentFrame.end();
}

bool CollisionService::WasOverlapping(ProxyHandle a, ProxyHandle b) const
{
	return PreviousFrame.find(MakeOrderedPair(a, b)) != PreviousFrame.end();
}

void CollisionService::GetCollisionEnters(std::vector<CollisionPair>& outEnters) const
{
	outEnters.clear();
	for (const auto& pair : CurrentFrame) {
		if (PreviousFrame.find(pair) == PreviousFrame.end()) {
			outEnters.push_back(pair);
		}
	}
}

void CollisionService::GetCollisionStays(std::vector<CollisionPair>& outStays) const
{
	outStays.clear();
	for (const auto& pair : CurrentFrame) {
		if (PreviousFrame.find(pair) != PreviousFrame.end()) {
			outStays.push_back(pair);
		}
	}
}

void CollisionService::GetCollisionExits(std::vector<CollisionPair>& outExits) const
{
	outExits.clear();
	for (const auto& pair : PreviousFrame) {
		if (CurrentFrame.find(pair) == CurrentFrame.end()) {
			outExits.push_back(pair);
		}
	}
}

CollisionPair CollisionService::MakeOrderedPair(ProxyHandle a, ProxyHandle b)
{
	CollisionPair pair;
	if (a < b) {
		pair.A = a;
		pair.B = b;
	} else {
		pair.A = b;
		pair.B = a;
	}
	return pair;
}
