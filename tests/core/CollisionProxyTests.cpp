#include <gtest/gtest.h>
#include <collision/QuadTreeProxy.h>

TEST(QuadTreeProxyTest, DefaultConstruction) {
    QuadTreeProxy proxy;
    
    EXPECT_EQ(proxy.Handle, InvalidProxyHandle);
    EXPECT_TRUE(proxy.Enabled);
    EXPECT_FALSE(proxy.IsTrigger);
    EXPECT_TRUE(proxy.Dirty);
    EXPECT_EQ(proxy.LayerMask, CollisionLayer::Default);
    EXPECT_EQ(proxy.CollisionMask, CollisionLayer::All);
    EXPECT_EQ(proxy.UserData, nullptr);
}

TEST(QuadTreeProxyTest, CanCollideWithSameLayer) {
    QuadTreeProxy a;
    a.LayerMask = CollisionLayer::Player;
    a.CollisionMask = CollisionLayer::Enemy;
    
    QuadTreeProxy b;
    b.LayerMask = CollisionLayer::Enemy;
    b.CollisionMask = CollisionLayer::Player;
    
    EXPECT_TRUE(a.CanCollideWith(b));
    EXPECT_TRUE(b.CanCollideWith(a));
}

TEST(QuadTreeProxyTest, CannotCollideWithDifferentLayers) {
    QuadTreeProxy a;
    a.LayerMask = CollisionLayer::Player;
    a.CollisionMask = CollisionLayer::Enemy;
    
    QuadTreeProxy b;
    b.LayerMask = CollisionLayer::Environment;
    b.CollisionMask = CollisionLayer::Projectile;
    
    EXPECT_FALSE(a.CanCollideWith(b));
    EXPECT_FALSE(b.CanCollideWith(a));
}

TEST(QuadTreeProxyTest, CollisionMaskAll) {
    QuadTreeProxy a;
    a.LayerMask = CollisionLayer::Player;
    a.CollisionMask = CollisionLayer::All;
    
    QuadTreeProxy b;
    b.LayerMask = CollisionLayer::Enemy;
    b.CollisionMask = CollisionLayer::All;
    
    EXPECT_TRUE(a.CanCollideWith(b));
    EXPECT_TRUE(b.CanCollideWith(a));
}

TEST(QuadTreeProxyTest, CollisionMaskNone) {
    QuadTreeProxy a;
    a.LayerMask = CollisionLayer::Player;
    a.CollisionMask = CollisionLayer::None;
    
    QuadTreeProxy b;
    b.LayerMask = CollisionLayer::Enemy;
    b.CollisionMask = CollisionLayer::All;
    
    EXPECT_FALSE(a.CanCollideWith(b));
    // b can collide with a (b's mask includes all)
    // but a cannot collide with b (a's mask is none)
}

TEST(QuadTreeProxyTest, MarkDirtyIfMovedDetectsChange) {
    QuadTreeProxy proxy;
    proxy.Bounds = Rectf(0.0f, 0.0f, 10.0f, 10.0f);
    proxy.PreviousBounds = proxy.Bounds;
    proxy.Dirty = false;
    
    // Move the proxy
    proxy.Bounds.x = 5.0f;
    proxy.MarkDirtyIfMoved();
    
    EXPECT_TRUE(proxy.Dirty);
    EXPECT_FLOAT_EQ(proxy.PreviousBounds.x, 5.0f);
}

TEST(QuadTreeProxyTest, MarkDirtyIfMovedNoChange) {
    QuadTreeProxy proxy;
    proxy.Bounds = Rectf(0.0f, 0.0f, 10.0f, 10.0f);
    proxy.PreviousBounds = proxy.Bounds;
    proxy.Dirty = false;
    
    // No movement
    proxy.MarkDirtyIfMoved();
    
    EXPECT_FALSE(proxy.Dirty);
}

TEST(QuadTreeProxyTest, MarkDirtyIfMovedSizeChange) {
    QuadTreeProxy proxy;
    proxy.Bounds = Rectf(0.0f, 0.0f, 10.0f, 10.0f);
    proxy.PreviousBounds = proxy.Bounds;
    proxy.Dirty = false;
    
    // Resize
    proxy.Bounds.width = 20.0f;
    proxy.MarkDirtyIfMoved();
    
    EXPECT_TRUE(proxy.Dirty);
}

TEST(CollisionLayerTest, LayerValues) {
    EXPECT_EQ(CollisionLayer::None, 0u);
    EXPECT_EQ(CollisionLayer::Default, 1u);
    EXPECT_EQ(CollisionLayer::Player, 2u);
    EXPECT_EQ(CollisionLayer::Enemy, 4u);
    EXPECT_EQ(CollisionLayer::Projectile, 8u);
    EXPECT_EQ(CollisionLayer::Environment, 16u);
    EXPECT_EQ(CollisionLayer::Trigger, 32u);
    EXPECT_EQ(CollisionLayer::All, 0xFFFFFFFFu);
}

TEST(CollisionLayerTest, LayerBitmaskCombination) {
    uint32_t playerAndEnemy = CollisionLayer::Player | CollisionLayer::Enemy;
    
    EXPECT_TRUE((playerAndEnemy & CollisionLayer::Player) != 0);
    EXPECT_TRUE((playerAndEnemy & CollisionLayer::Enemy) != 0);
    EXPECT_FALSE((playerAndEnemy & CollisionLayer::Projectile) != 0);
}

TEST(QuadTreeProxyTest, InvalidProxyHandleIsZero) {
    EXPECT_EQ(InvalidProxyHandle, 0u);
}
