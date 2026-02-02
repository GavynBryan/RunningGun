#include <core/base/Actor.h>
#include <core/components/TransformComponent.h>
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

void Actor::AttachComponent(std::unique_ptr<ActorComponent> comp)
{
	// Get type and raw pointer before moving
	std::type_index typeIdx(typeid(*comp));
	ActorComponent* rawPtr = comp.get();

	// Add to type-indexed map (which owns the component)
	ComponentsByType[typeIdx].push_back(std::move(comp));

	// Add to flat list for iteration order
	AllComponents.push_back(rawPtr);

	// Cache transform if this is a TransformComponent
	if (!Transform) {
		Transform = GetComponent<TransformComponent>();
	}

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

	// Invalidate Transform cache if needed
	if (target == Transform) {
		Transform = nullptr;
	}

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

	// Detach from current parent
	if (Parent) {
		Parent->RemoveChild(this);
	}

	Parent = newParent;

	// Attach to new parent
	if (Parent) {
		Parent->AddChild(this);
	}

	// Sync Transform hierarchy
	EnsureTransform();
	if (Transform) {
		TransformComponent* parentTransform = nullptr;
		if (Parent) {
			const_cast<Actor*>(Parent)->EnsureTransform();
			parentTransform = Parent->Transform;
		}
		Transform->SetParent(parentTransform, preserveWorldPosition);
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

void Actor::EnsureTransform()
{
	if (!Transform) {
		Transform = GetComponent<TransformComponent>();
	}
}

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
	EnsureTransform();
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

void Actor::SetPosition(const Vec2& pos)
{
	EnsureTransform();
	if (Transform) {
		Transform->SetPosition(pos);
	}
}

void Actor::SetPosition(float x, float y)
{
	EnsureTransform();
	if (Transform) {
		Transform->SetPosition(x, y);
	}
}

Vec2 Actor::GetPosition() const
{
	const_cast<Actor*>(this)->EnsureTransform();
	if (Transform) {
		return Transform->GetPosition();
	}
	return Vec2(0.0f, 0.0f);
}

void Actor::SetLocalPosition(const Vec2& pos)
{
	EnsureTransform();
	if (Transform) {
		Transform->SetLocalPosition(pos);
	}
}

void Actor::SetLocalPosition(float x, float y)
{
	EnsureTransform();
	if (Transform) {
		Transform->SetLocalPosition(x, y);
	}
}

Vec2 Actor::GetLocalPosition() const
{
	const_cast<Actor*>(this)->EnsureTransform();
	if (Transform) {
		return Transform->GetLocalPosition();
	}
	return Vec2(0.0f, 0.0f);
}

void Actor::SetDirection(const Vec2& dir)
{
	EnsureTransform();
	if (Transform) {
		Transform->SetDirection(dir);
	}
}

void Actor::SetDirection(float x, float y)
{
	EnsureTransform();
	if (Transform) {
		Transform->SetDirection(x, y);
	}
}

Vec2 Actor::GetDirection() const
{
	const_cast<Actor*>(this)->EnsureTransform();
	if (Transform) {
		return Transform->GetDirection();
	}
	return Vec2(1.0f, 0.0f);
}

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
