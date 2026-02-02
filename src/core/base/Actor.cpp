#include <core/base/Actor.h>
#include <core/components/SpriteComponent.h>
#include <typeindex>
#include <algorithm>

Actor::Actor()
{
}

Actor::~Actor()
{
	// Detach from parent
	if (Parent) {
		Parent->RemoveChild(this);
		Parent = nullptr;
	}

	// Detach all children (they become root-level)
	DetachAllChildren();
}

// ========== Component Management ==========

void Actor::AttachComponent(std::unique_ptr<ActorComponent> comp)
{
	// Get type and raw pointer before moving
	std::type_index typeIdx(typeid(*comp));
	ActorComponent* rawPtr = comp.get();

	// Add to type-indexed map (which owns the component)
	ComponentsByType[typeIdx].push_back(std::move(comp));

	// Add to flat list for iteration order
	AllComponents.push_back(rawPtr);

	// Notify subscribers of component change
	OnComponentsChanged.Broadcast();
}

bool Actor::RemoveComponent(ActorComponent& comp)
{
	ActorComponent* target = &comp;
	std::type_index typeIdx(typeid(comp));

	// Find and remove from type bucket
	auto bucketIt = ComponentsByType.find(typeIdx);
	if (bucketIt == ComponentsByType.end()) {
		return false;
	}

	auto& bucket = bucketIt->second;
	auto compIt = std::find_if(bucket.begin(), bucket.end(),
		[target](const std::unique_ptr<ActorComponent>& ptr) {
			return ptr.get() == target;
		});

	if (compIt == bucket.end()) {
		return false;
	}

	// Remove from AllComponents
	AllComponents.erase(
		std::remove(AllComponents.begin(), AllComponents.end(), target),
		AllComponents.end()
	);

	// Remove from bucket (this destroys the component)
	bucket.erase(compIt);

	// Clean up empty bucket
	if (bucket.empty()) {
		ComponentsByType.erase(bucketIt);
	}

	// Notify subscribers of component change
	OnComponentsChanged.Broadcast();

	return true;
}

// ========== Hierarchy Management ==========

void Actor::SetParent(Actor* newParent, bool preserveWorldPosition)
{
	// No-op if same parent
	if (Parent == newParent) {
		return;
	}

	// Prevent circular hierarchy
	if (newParent && newParent->IsDescendantOf(this)) {
		return;
	}

	// Cache world transform before re-parenting
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
			LocalTransform.Position = WorldToLocal(worldPos);
			// Approximate scale/rotation preservation
			Vec2 parentScale = Parent->GetScale();
			if (parentScale.x != 0.0f && parentScale.y != 0.0f) {
				LocalTransform.Scale.x = worldScale.x / parentScale.x;
				LocalTransform.Scale.y = worldScale.y / parentScale.y;
			}
			LocalTransform.Rotation = worldRot - Parent->GetRotation();
		} else {
			// Becoming root - local becomes world
			LocalTransform.Position = worldPos;
			LocalTransform.Scale = worldScale;
			LocalTransform.Rotation = worldRot;
		}
	}

	// Notify subscribers
	OnHierarchyChanged.Broadcast();
}

void Actor::DetachAllChildren()
{
	// Copy vector since SetParent modifies Children
	std::vector<Actor*> childrenCopy = Children;
	for (auto* child : childrenCopy) {
		child->SetParent(nullptr, true);
	}
}

bool Actor::IsDescendantOf(const Actor* ancestor) const
{
	const Actor* current = Parent;
	while (current) {
		if (current == ancestor) {
			return true;
		}
		current = current->Parent;
	}
	return false;
}

void Actor::SetEnabledRecursive(bool enabled)
{
	if (enabled) {
		Enable();
	} else {
		Disable();
	}

	for (auto* child : Children) {
		child->SetEnabledRecursive(enabled);
	}
}

void Actor::AddChild(Actor* child)
{
	if (child && std::find(Children.begin(), Children.end(), child) == Children.end()) {
		Children.push_back(child);
		OnHierarchyChanged.Broadcast();
	}
}

void Actor::RemoveChild(Actor* child)
{
	auto it = std::find(Children.begin(), Children.end(), child);
	if (it != Children.end()) {
		Children.erase(it);
		OnHierarchyChanged.Broadcast();
	}
}

// ========== Lifecycle ==========

void Actor::StartComponents()
{
	for (auto* component : AllComponents) {
		component->Start();
	}
}

void Actor::UpdateComponents()
{
	for (auto* component : AllComponents) {
		component->Update();
	}
}

void Actor::Start()
{
	if (Activated) {
		StartComponents();
	}
}

void Actor::Update()
{
	if (Activated) {
		UpdateComponents();
	}
}

// ========== World Position ==========

void Actor::SetPosition(const Vec2& position)
{
	if (Parent) {
		LocalTransform.Position = Parent->WorldToLocal(position);
	} else {
		LocalTransform.Position = position;
	}
}

void Actor::SetPosition(float x, float y)
{
	SetPosition(Vec2(x, y));
}

Vec2 Actor::GetPosition() const
{
	return LocalToWorld(Vec2(0.0f, 0.0f));
}

Vec2& Actor::GetPosition()
{
	CachedWorldPosition = LocalToWorld(Vec2(0.0f, 0.0f));
	return CachedWorldPosition;
}

// ========== World Scale ==========

Vec2 Actor::GetScale() const
{
	if (Parent) {
		Vec2 parentScale = Parent->GetScale();
		return Vec2(LocalTransform.Scale.x * parentScale.x, LocalTransform.Scale.y * parentScale.y);
	}
	return LocalTransform.Scale;
}

// ========== World Rotation ==========

float Actor::GetRotation() const
{
	if (Parent) {
		return LocalTransform.Rotation + Parent->GetRotation();
	}
	return LocalTransform.Rotation;
}

// ========== Transform2D ==========

Transform2D Actor::GetTransform() const
{
	return Transform2D(GetPosition(), GetScale(), GetRotation());
}

Transform2D& Actor::GetTransform()
{
	CachedWorldTransform = Transform2D(GetPosition(), GetScale(), GetRotation());
	return CachedWorldTransform;
}

// ========== Coordinate Transforms ==========

Vec2 Actor::LocalToWorld(const Vec2& localPoint) const
{
	// Use Transform2D's LocalToWorld for the local transform math
	Vec2 translated = LocalTransform.LocalToWorld(localPoint);

	// Recurse to parent if present
	if (Parent) {
		return Parent->LocalToWorld(translated);
	}
	return translated;
}

Vec2 Actor::WorldToLocal(const Vec2& worldPoint) const
{
	Vec2 point = worldPoint;

	// Transform from parent space first
	if (Parent) {
		point = Parent->WorldToLocal(point);
	}

	// Use Transform2D's WorldToLocal for the local transform math
	return LocalTransform.WorldToLocal(point);
}

// ========== Sprite Convenience ==========

void Actor::SetFlipX(bool flip)
{
	if (auto* sprite = GetComponent<SpriteComponent>()) {
		sprite->SetFlipX(flip);
	}
}

bool Actor::GetFlipX() const
{
	if (const auto* sprite = GetComponent<SpriteComponent>()) {
		return sprite->GetFlipX();
	}
	return false;
}

float Actor::GetWidth() const
{
	if (const auto* sprite = GetComponent<SpriteComponent>()) {
		return sprite->GetWidth();
	}
	return 0.0f;
}

float Actor::GetHeight() const
{
	if (const auto* sprite = GetComponent<SpriteComponent>()) {
		return sprite->GetHeight();
	}
	return 0.0f;
}

void Actor::OnCollide(Actor& other)
{
	if (Activated) {
		for (auto* component : AllComponents) {
			component->OnCollide(other);
		}
	}
}
