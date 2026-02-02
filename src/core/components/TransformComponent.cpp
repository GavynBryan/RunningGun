#include <core/components/TransformComponent.h>
#include <cmath>

TransformComponent::TransformComponent(Actor& entity, GameServiceHost& services)
	: ActorComponent(entity, services)
{
}

TransformComponent::~TransformComponent()
{
	// Detach from parent
	if (Parent) {
		Parent->RemoveChild(this);
		Parent = nullptr;
	}

	// Detach all children (they become root-level, preserving world position)
	DetachAllChildren();
}

// ========== Hierarchy Management ==========

void TransformComponent::SetParent(TransformComponent* newParent, bool preserveWorldPosition)
{
	// No-op if same parent
	if (Parent == newParent) {
		return;
	}

	// Prevent circular hierarchy
	if (newParent && newParent->IsDescendantOf(this)) {
		return;
	}

	Vec2 worldPos = GetPosition();
	Vec2 worldScale = GetScale();
	float worldRot = GetRotation();

	// Detach from current parent
	if (Parent) {
		Parent->RemoveChild(this);
	}

	Parent = newParent;

	// Attach to new parent
	if (Parent) {
		Parent->AddChild(this);
	}

	// Preserve world transform by adjusting local values
	if (preserveWorldPosition) {
		if (Parent) {
			LocalPosition = WorldToLocal(worldPos);
			// Approximate scale/rotation preservation (simple case)
			Vec2 parentScale = Parent->GetScale();
			if (parentScale.x != 0.0f && parentScale.y != 0.0f) {
				LocalScale.x = worldScale.x / parentScale.x;
				LocalScale.y = worldScale.y / parentScale.y;
			}
			LocalRotation = worldRot - Parent->GetRotation();
		} else {
			// Becoming root - local becomes world
			LocalPosition = worldPos;
			LocalScale = worldScale;
			LocalRotation = worldRot;
		}
	}
}

void TransformComponent::DetachAllChildren()
{
	// Copy vector since SetParent modifies Children
	std::vector<TransformComponent*> childrenCopy = Children;
	for (auto* child : childrenCopy) {
		child->SetParent(nullptr, true);
	}
}

bool TransformComponent::IsDescendantOf(const TransformComponent* ancestor) const
{
	const TransformComponent* current = Parent;
	while (current) {
		if (current == ancestor) {
			return true;
		}
		current = current->Parent;
	}
	return false;
}

void TransformComponent::AddChild(TransformComponent* child)
{
	if (child && std::find(Children.begin(), Children.end(), child) == Children.end()) {
		Children.push_back(child);
	}
}

void TransformComponent::RemoveChild(TransformComponent* child)
{
	auto it = std::find(Children.begin(), Children.end(), child);
	if (it != Children.end()) {
		Children.erase(it);
	}
}

// ========== World Position ==========

void TransformComponent::SetPosition(const Vec2& position)
{
	if (Parent) {
		LocalPosition = WorldToLocal(position);
	} else {
		LocalPosition = position;
	}
}

void TransformComponent::SetPosition(float x, float y)
{
	SetPosition(Vec2(x, y));
}

Vec2 TransformComponent::GetPosition() const
{
	return LocalToWorld(Vec2(0.0f, 0.0f));
}

Vec2& TransformComponent::GetPosition()
{
	CachedWorldPosition = LocalToWorld(Vec2(0.0f, 0.0f));
	return CachedWorldPosition;
}

// ========== World Scale ==========

Vec2 TransformComponent::GetScale() const
{
	if (Parent) {
		Vec2 parentScale = Parent->GetScale();
		return Vec2(LocalScale.x * parentScale.x, LocalScale.y * parentScale.y);
	}
	return LocalScale;
}

// ========== World Rotation ==========

float TransformComponent::GetRotation() const
{
	if (Parent) {
		return LocalRotation + Parent->GetRotation();
	}
	return LocalRotation;
}

// ========== Coordinate Transforms ==========

Vec2 TransformComponent::LocalToWorld(const Vec2& localPoint) const
{
	// Apply local scale
	Vec2 scaled(localPoint.x * LocalScale.x, localPoint.y * LocalScale.y);

	// Apply local rotation
	float radians = LocalRotation * 3.14159265358979f / 180.0f;
	float cosR = std::cos(radians);
	float sinR = std::sin(radians);
	Vec2 rotated(
		scaled.x * cosR - scaled.y * sinR,
		scaled.x * sinR + scaled.y * cosR
	);

	// Apply local translation
	Vec2 translated = rotated + LocalPosition;

	// Recurse to parent if present
	if (Parent) {
		return Parent->LocalToWorld(translated);
	}
	return translated;
}

Vec2 TransformComponent::WorldToLocal(const Vec2& worldPoint) const
{
	Vec2 point = worldPoint;

	// Transform from parent space first
	if (Parent) {
		point = Parent->WorldToLocal(point);
	}

	// Reverse local translation
	point = point - LocalPosition;

	// Reverse local rotation
	float radians = -LocalRotation * 3.14159265358979f / 180.0f;
	float cosR = std::cos(radians);
	float sinR = std::sin(radians);
	Vec2 unrotated(
		point.x * cosR - point.y * sinR,
		point.x * sinR + point.y * cosR
	);

	// Reverse local scale
	Vec2 unscaled(
		LocalScale.x != 0.0f ? unrotated.x / LocalScale.x : 0.0f,
		LocalScale.y != 0.0f ? unrotated.y / LocalScale.y : 0.0f
	);

	return unscaled;
}
