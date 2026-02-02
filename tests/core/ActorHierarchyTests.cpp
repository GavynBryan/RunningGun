#include <gtest/gtest.h>
#include <core/base/Actor.h>
#include <core/components/TransformComponent.h>
#include <core/services/framework/GameServiceHost.h>
#include <core/services/logging/LoggingService.h>
#include <cmath>

class ActorHierarchyTest : public ::testing::Test
{
protected:
    void SetUp() override {
        Services.AddService<LoggingService>();
    }

    GameServiceHost Services;

    // Helper to compare floats with tolerance
    static bool NearEqual(float a, float b, float epsilon = 0.001f) {
        return std::abs(a - b) < epsilon;
    }

    static bool NearEqual(const Vec2& a, const Vec2& b, float epsilon = 0.001f) {
        return NearEqual(a.x, b.x, epsilon) && NearEqual(a.y, b.y, epsilon);
    }
};

// ============== Basic Hierarchy Tests ==============

TEST_F(ActorHierarchyTest, SetParentEstablishesRelationship) {
    Actor parent;
    Actor child;

    child.SetParent(&parent);

    EXPECT_EQ(child.GetParent(), &parent);
    EXPECT_TRUE(child.HasParent());
    EXPECT_TRUE(parent.HasChildren());
    EXPECT_EQ(parent.GetChildCount(), 1);
    EXPECT_EQ(parent.GetChildren()[0], &child);
}

TEST_F(ActorHierarchyTest, SetParentNullDetaches) {
    Actor parent;
    Actor child;

    child.SetParent(&parent);
    child.SetParent(nullptr);

    EXPECT_EQ(child.GetParent(), nullptr);
    EXPECT_FALSE(child.HasParent());
    EXPECT_FALSE(parent.HasChildren());
    EXPECT_EQ(parent.GetChildCount(), 0);
}

TEST_F(ActorHierarchyTest, DetachAllChildrenWorks) {
    Actor parent;
    Actor child1, child2, child3;

    child1.SetParent(&parent);
    child2.SetParent(&parent);
    child3.SetParent(&parent);

    EXPECT_EQ(parent.GetChildCount(), 3);

    parent.DetachAllChildren();

    EXPECT_EQ(parent.GetChildCount(), 0);
    EXPECT_EQ(child1.GetParent(), nullptr);
    EXPECT_EQ(child2.GetParent(), nullptr);
    EXPECT_EQ(child3.GetParent(), nullptr);
}

TEST_F(ActorHierarchyTest, PreventCircularHierarchy) {
    Actor grandparent;
    Actor parent;
    Actor child;

    parent.SetParent(&grandparent);
    child.SetParent(&parent);

    // Attempt to create cycle: grandparent -> child (should be rejected)
    grandparent.SetParent(&child);

    // Hierarchy should remain unchanged
    EXPECT_EQ(grandparent.GetParent(), nullptr);
    EXPECT_EQ(parent.GetParent(), &grandparent);
    EXPECT_EQ(child.GetParent(), &parent);
}

TEST_F(ActorHierarchyTest, IsDescendantOfWorks) {
    Actor grandparent;
    Actor parent;
    Actor child;
    Actor unrelated;

    parent.SetParent(&grandparent);
    child.SetParent(&parent);

    EXPECT_TRUE(child.IsDescendantOf(&parent));
    EXPECT_TRUE(child.IsDescendantOf(&grandparent));
    EXPECT_TRUE(parent.IsDescendantOf(&grandparent));

    EXPECT_FALSE(grandparent.IsDescendantOf(&child));
    EXPECT_FALSE(child.IsDescendantOf(&unrelated));
    EXPECT_FALSE(child.IsDescendantOf(&child));  // Not descendant of self
}

TEST_F(ActorHierarchyTest, SetEnabledRecursiveWorks) {
    Actor parent;
    Actor child1, child2;
    Actor grandchild;

    child1.SetParent(&parent);
    child2.SetParent(&parent);
    grandchild.SetParent(&child1);

    parent.SetEnabledRecursive(false);

    EXPECT_FALSE(parent.IsEnabled());
    EXPECT_FALSE(child1.IsEnabled());
    EXPECT_FALSE(child2.IsEnabled());
    EXPECT_FALSE(grandchild.IsEnabled());

    parent.SetEnabledRecursive(true);

    EXPECT_TRUE(parent.IsEnabled());
    EXPECT_TRUE(child1.IsEnabled());
    EXPECT_TRUE(child2.IsEnabled());
    EXPECT_TRUE(grandchild.IsEnabled());
}

// ============== Position Offset Tests ==============

TEST_F(ActorHierarchyTest, ChildLocalPositionIsOffset) {
    Actor parent;
    Actor child;

    parent.SetPosition(100.0f, 100.0f);
    child.SetParent(&parent);
    child.SetLocalPosition(50.0f, 25.0f);

    Vec2 worldPos = child.GetPosition();
    EXPECT_TRUE(NearEqual(worldPos, Vec2(150.0f, 125.0f)));
}

TEST_F(ActorHierarchyTest, WorldPositionComputesThroughHierarchy) {
    Actor grandparent;
    Actor parent;
    Actor child;

    grandparent.SetPosition(100.0f, 100.0f);
    parent.SetParent(&grandparent);
    parent.SetLocalPosition(50.0f, 50.0f);
    child.SetParent(&parent);
    child.SetLocalPosition(10.0f, 10.0f);

    Vec2 parentWorld = parent.GetPosition();
    Vec2 childWorld = child.GetPosition();

    EXPECT_TRUE(NearEqual(parentWorld, Vec2(150.0f, 150.0f)));
    EXPECT_TRUE(NearEqual(childWorld, Vec2(160.0f, 160.0f)));
}

TEST_F(ActorHierarchyTest, SetWorldPositionAdjustsLocalOffset) {
    Actor parent;
    Actor child;

    parent.SetPosition(100.0f, 100.0f);
    child.SetParent(&parent);

    // Set world position, should compute correct local offset
    child.SetPosition(200.0f, 150.0f);

    Vec2 localPos = child.GetLocalPosition();
    EXPECT_TRUE(NearEqual(localPos, Vec2(100.0f, 50.0f)));
}

TEST_F(ActorHierarchyTest, ParentMovementUpdatesChildWorldPosition) {
    Actor parent;
    Actor child;

    parent.SetPosition(100.0f, 100.0f);
    child.SetParent(&parent);
    child.SetLocalPosition(50.0f, 50.0f);

    // Move parent
    parent.SetPosition(200.0f, 200.0f);

    // Child world position should update
    Vec2 childWorld = child.GetPosition();
    EXPECT_TRUE(NearEqual(childWorld, Vec2(250.0f, 250.0f)));

    // Child local position should remain unchanged
    Vec2 childLocal = child.GetLocalPosition();
    EXPECT_TRUE(NearEqual(childLocal, Vec2(50.0f, 50.0f)));
}

TEST_F(ActorHierarchyTest, DetachPreservesWorldPosition) {
    Actor parent;
    Actor child;

    parent.SetPosition(100.0f, 100.0f);
    child.SetParent(&parent);
    child.SetLocalPosition(50.0f, 50.0f);

    Vec2 worldBefore = child.GetPosition();

    // Detach with preserveWorldPosition = true (default)
    child.SetParent(nullptr);

    Vec2 worldAfter = child.GetPosition();
    EXPECT_TRUE(NearEqual(worldBefore, worldAfter));
}

TEST_F(ActorHierarchyTest, ReparentPreservesWorldPosition) {
    Actor parent1, parent2;
    Actor child;

    parent1.SetPosition(100.0f, 100.0f);
    parent2.SetPosition(200.0f, 50.0f);

    child.SetParent(&parent1);
    child.SetLocalPosition(50.0f, 50.0f);

    Vec2 worldBefore = child.GetPosition();

    // Reparent to different parent
    child.SetParent(&parent2);

    Vec2 worldAfter = child.GetPosition();
    EXPECT_TRUE(NearEqual(worldBefore, worldAfter));

    // Local position should have been adjusted
    Vec2 localAfter = child.GetLocalPosition();
    EXPECT_TRUE(NearEqual(localAfter, Vec2(-50.0f, 100.0f)));
}

// ============== TransformComponent Tests ==============

TEST_F(ActorHierarchyTest, TransformHierarchySyncsWithActorHierarchy) {
    Actor parent;
    Actor child;

    auto* parentTransform = parent.GetTransformComponent();
    auto* childTransform = child.GetTransformComponent();

    child.SetParent(&parent);

    EXPECT_EQ(childTransform->GetParent(), parentTransform);
}

TEST_F(ActorHierarchyTest, TransformLocalToWorldWorks) {
    Actor parent;
    parent.SetPosition(100.0f, 100.0f);

    auto* transform = parent.GetTransformComponent();
    Vec2 worldPoint = transform->LocalToWorld(Vec2(50.0f, 50.0f));

    EXPECT_TRUE(NearEqual(worldPoint, Vec2(150.0f, 150.0f)));
}

TEST_F(ActorHierarchyTest, TransformWorldToLocalWorks) {
    Actor parent;
    parent.SetPosition(100.0f, 100.0f);

    auto* transform = parent.GetTransformComponent();
    Vec2 localPoint = transform->WorldToLocal(Vec2(150.0f, 150.0f));

    EXPECT_TRUE(NearEqual(localPoint, Vec2(50.0f, 50.0f)));
}

TEST_F(ActorHierarchyTest, ScaleInheritsFromParent) {
    Actor parent;
    Actor child;

    parent.GetTransformComponent()->SetLocalScale(2.0f, 2.0f);
    child.SetParent(&parent);
    child.GetTransformComponent()->SetLocalScale(0.5f, 0.5f);

    Vec2 worldScale = child.GetTransformComponent()->GetScale();
    EXPECT_TRUE(NearEqual(worldScale, Vec2(1.0f, 1.0f)));
}

TEST_F(ActorHierarchyTest, RotationInheritsFromParent) {
    Actor parent;
    Actor child;

    parent.GetTransformComponent()->SetLocalRotation(45.0f);
    child.SetParent(&parent);
    child.GetTransformComponent()->SetLocalRotation(30.0f);

    float worldRotation = child.GetTransformComponent()->GetRotation();
    EXPECT_NEAR(worldRotation, 75.0f, 0.001f);
}

// ============== Edge Cases ==============

TEST_F(ActorHierarchyTest, SetSameParentIsNoOp) {
    Actor parent;
    Actor child;

    child.SetParent(&parent);
    child.SetLocalPosition(50.0f, 50.0f);

    // Set same parent again
    child.SetParent(&parent);

    // Position should be unchanged
    Vec2 localPos = child.GetLocalPosition();
    EXPECT_TRUE(NearEqual(localPos, Vec2(50.0f, 50.0f)));
    EXPECT_EQ(parent.GetChildCount(), 1);
}

TEST_F(ActorHierarchyTest, MultipleChildrenWork) {
    Actor parent;
    Actor child1, child2, child3;

    parent.SetPosition(100.0f, 100.0f);

    child1.SetParent(&parent);
    child1.SetLocalPosition(10.0f, 0.0f);

    child2.SetParent(&parent);
    child2.SetLocalPosition(20.0f, 0.0f);

    child3.SetParent(&parent);
    child3.SetLocalPosition(30.0f, 0.0f);

    EXPECT_EQ(parent.GetChildCount(), 3);
    EXPECT_TRUE(NearEqual(child1.GetPosition(), Vec2(110.0f, 100.0f)));
    EXPECT_TRUE(NearEqual(child2.GetPosition(), Vec2(120.0f, 100.0f)));
    EXPECT_TRUE(NearEqual(child3.GetPosition(), Vec2(130.0f, 100.0f)));
}

TEST_F(ActorHierarchyTest, DeepHierarchyWorksCorrectly) {
    constexpr int DEPTH = 10;
    std::vector<Actor> actors(DEPTH);

    // Build chain: actor[0] -> actor[1] -> ... -> actor[9]
    actors[0].SetPosition(100.0f, 100.0f);
    for (int i = 1; i < DEPTH; ++i) {
        actors[i].SetParent(&actors[i - 1]);
        actors[i].SetLocalPosition(10.0f, 10.0f);
    }

    // Last actor should be at (100 + 10*9, 100 + 10*9) = (190, 190)
    Vec2 deepPos = actors[DEPTH - 1].GetPosition();
    EXPECT_TRUE(NearEqual(deepPos, Vec2(190.0f, 190.0f)));
}

// ============== Callback Tests ==============

TEST_F(ActorHierarchyTest, OnHierarchyChangedFires) {
    Actor parent;
    Actor child;

    int callCount = 0;
    child.OnHierarchyChanged.Subscribe([&callCount]() {
        callCount++;
    });

    child.SetParent(&parent);
    EXPECT_EQ(callCount, 1);

    child.SetParent(nullptr);
    EXPECT_EQ(callCount, 2);
}
