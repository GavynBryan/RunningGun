#pragma once

#include <core/collision/QuadTreeProxy.h>
#include <core/framework/IService.h>
#include <unordered_map>

class RigidBody2DComponent;

// Registry for RigidBody2D components.
// Maps proxy handles to rigidbody references for collision response.
// Note: Proxies are owned by CollisionService, not PhysicsService.
class PhysicsService final : public IService
{
public:
	PhysicsService() = default;

	// RigidBody registration - associates a proxy handle with a rigidbody
	void RegisterRigidBody(ProxyHandle handle, RigidBody2DComponent* body);
	void UnregisterRigidBody(ProxyHandle handle);

	// Lookup rigidbody by proxy handle (for collision response)
	RigidBody2DComponent* GetRigidBody(ProxyHandle handle) const;

private:
	std::unordered_map<ProxyHandle, RigidBody2DComponent*> HandleToBody;
};
