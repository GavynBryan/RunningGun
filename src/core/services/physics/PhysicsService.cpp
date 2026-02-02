#include <core/services/physics/PhysicsService.h>

void PhysicsService::RegisterRigidBody(ProxyHandle handle, RigidBody2DComponent* body)
{
	if (handle != InvalidProxyHandle && body != nullptr) {
		HandleToBody[handle] = body;
	}
}

void PhysicsService::UnregisterRigidBody(ProxyHandle handle)
{
	HandleToBody.erase(handle);
}

RigidBody2DComponent* PhysicsService::GetRigidBody(ProxyHandle handle) const
{
	auto it = HandleToBody.find(handle);
	return (it != HandleToBody.end()) ? it->second : nullptr;
}
