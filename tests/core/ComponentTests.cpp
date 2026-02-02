#include <gtest/gtest.h>
#include <core/base/Actor.h>
#include <core/components/SpriteComponent.h>
#include <core/services/framework/GameServiceHost.h>
#include <core/services/logging/LoggingService.h>

class ActorTest : public ::testing::Test
{
protected:
    void SetUp() override {
        Services.AddService<LoggingService>();
    }

    GameServiceHost Services;
};

TEST_F(ActorTest, DefaultConstruction) {
    Actor actor;
    
    EXPECT_TRUE(actor.IsEnabled());
    EXPECT_EQ(actor.GetTag(), ACTOR_TAG::player);
}

TEST_F(ActorTest, DefaultTransformValues) {
    Actor actor;
    
    // Actor has embedded transform with default values
    EXPECT_FLOAT_EQ(actor.GetPosition().x, 0.0f);
    EXPECT_FLOAT_EQ(actor.GetPosition().y, 0.0f);
    EXPECT_FLOAT_EQ(actor.GetScale().x, 1.0f);
    EXPECT_FLOAT_EQ(actor.GetScale().y, 1.0f);
    EXPECT_FLOAT_EQ(actor.GetRotation(), 0.0f);
}

TEST_F(ActorTest, SetAndGetPosition) {
    Actor actor;
    
    actor.SetPosition(100.0f, 200.0f);
    Vec2 pos = actor.GetPosition();
    
    EXPECT_FLOAT_EQ(pos.x, 100.0f);
    EXPECT_FLOAT_EQ(pos.y, 200.0f);
}

TEST_F(ActorTest, SetPositionWithVec2) {
    Actor actor;
    
    actor.SetPosition(Vec2(50.0f, 75.0f));
    Vec2 pos = actor.GetPosition();
    
    EXPECT_FLOAT_EQ(pos.x, 50.0f);
    EXPECT_FLOAT_EQ(pos.y, 75.0f);
}

TEST_F(ActorTest, SetAndGetDirection) {
    Actor actor;
    
    actor.SetDirection(-1.0f, 0.0f);
    Vec2 dir = actor.GetDirection();
    
    EXPECT_FLOAT_EQ(dir.x, -1.0f);
    EXPECT_FLOAT_EQ(dir.y, 0.0f);
}

TEST_F(ActorTest, EnableDisable) {
    Actor actor;
    
    EXPECT_TRUE(actor.IsEnabled());
    
    actor.Disable();
    EXPECT_FALSE(actor.IsEnabled());
    
    actor.Enable();
    EXPECT_TRUE(actor.IsEnabled());
}

TEST_F(ActorTest, SetTag) {
    Actor actor;
    
    actor.SetTag(ACTOR_TAG::enemy_bullet);
    EXPECT_EQ(actor.GetTag(), ACTOR_TAG::enemy_bullet);
    
    actor.SetTag(ACTOR_TAG::hazard);
    EXPECT_EQ(actor.GetTag(), ACTOR_TAG::hazard);
}

TEST_F(ActorTest, AttachComponent) {
    Actor actor;
    
    auto sprite = std::make_unique<SpriteComponent>(actor, Services);
    SpriteComponent* rawPtr = sprite.get();
    
    actor.AttachComponent(std::move(sprite));
    
    SpriteComponent* retrieved = actor.GetComponent<SpriteComponent>();
    EXPECT_EQ(retrieved, rawPtr);
}

TEST_F(ActorTest, GetComponentReturnsNullWhenNotPresent) {
    Actor actor;
    
    // No SpriteComponent attached yet
    auto* sprite = actor.GetSprite();
    EXPECT_EQ(sprite, nullptr);
}

TEST_F(ActorTest, OnComponentsChangedBroadcasts) {
    Actor actor;
    int callCount = 0;
    
    actor.OnComponentsChanged.Subscribe([&callCount]() {
        callCount++;
    });
    
    auto sprite = std::make_unique<SpriteComponent>(actor, Services);
    actor.AttachComponent(std::move(sprite));
    
    EXPECT_EQ(callCount, 1);
}

// ============= Actor Transform Tests =============

TEST_F(ActorTest, TransformDefaultValues) {
    Actor actor;
    
    EXPECT_FLOAT_EQ(actor.GetPosition().x, 0.0f);
    EXPECT_FLOAT_EQ(actor.GetPosition().y, 0.0f);
    EXPECT_FLOAT_EQ(actor.GetDirection().x, 1.0f);  // Default facing right
    EXPECT_FLOAT_EQ(actor.GetDirection().y, 0.0f);
    EXPECT_FLOAT_EQ(actor.GetScale().x, 1.0f);
    EXPECT_FLOAT_EQ(actor.GetScale().y, 1.0f);
    EXPECT_FLOAT_EQ(actor.GetRotation(), 0.0f);
}

TEST_F(ActorTest, TransformSetPosition) {
    Actor actor;
    
    actor.SetPosition(100.0f, 200.0f);
    
    EXPECT_FLOAT_EQ(actor.GetPosition().x, 100.0f);
    EXPECT_FLOAT_EQ(actor.GetPosition().y, 200.0f);
}

TEST_F(ActorTest, TransformSetPositionVec2) {
    Actor actor;
    
    actor.SetPosition(Vec2(50.0f, 75.0f));
    
    EXPECT_FLOAT_EQ(actor.GetPosition().x, 50.0f);
    EXPECT_FLOAT_EQ(actor.GetPosition().y, 75.0f);
}

TEST_F(ActorTest, TransformTranslate) {
    Actor actor;
    actor.SetPosition(10.0f, 20.0f);
    
    actor.Translate(5.0f, 10.0f);
    
    EXPECT_FLOAT_EQ(actor.GetPosition().x, 15.0f);
    EXPECT_FLOAT_EQ(actor.GetPosition().y, 30.0f);
}

TEST_F(ActorTest, TransformTranslateVec2) {
    Actor actor;
    actor.SetPosition(10.0f, 20.0f);
    
    actor.Translate(Vec2(-5.0f, -10.0f));
    
    EXPECT_FLOAT_EQ(actor.GetPosition().x, 5.0f);
    EXPECT_FLOAT_EQ(actor.GetPosition().y, 10.0f);
}

TEST_F(ActorTest, TransformSetDirection) {
    Actor actor;
    
    actor.SetDirection(-1.0f, 0.0f);
    
    EXPECT_FLOAT_EQ(actor.GetDirection().x, -1.0f);
    EXPECT_FLOAT_EQ(actor.GetDirection().y, 0.0f);
}

TEST_F(ActorTest, TransformSetUniformScale) {
    Actor actor;
    
    actor.SetScale(2.0f);
    
    EXPECT_FLOAT_EQ(actor.GetScale().x, 2.0f);
    EXPECT_FLOAT_EQ(actor.GetScale().y, 2.0f);
}

TEST_F(ActorTest, TransformSetNonUniformScale) {
    Actor actor;
    
    actor.SetScale(2.0f, 3.0f);
    
    EXPECT_FLOAT_EQ(actor.GetScale().x, 2.0f);
    EXPECT_FLOAT_EQ(actor.GetScale().y, 3.0f);
}

TEST_F(ActorTest, TransformSetRotation) {
    Actor actor;
    
    actor.SetRotation(45.0f);
    
    EXPECT_FLOAT_EQ(actor.GetRotation(), 45.0f);
}

TEST_F(ActorTest, TransformRotate) {
    Actor actor;
    actor.SetRotation(10.0f);
    
    actor.Rotate(15.0f);
    
    EXPECT_FLOAT_EQ(actor.GetRotation(), 25.0f);
}

TEST_F(ActorTest, GetTransform2DReturnsCorrectValues) {
    Actor actor;
    actor.SetPosition(100.0f, 200.0f);
    actor.SetScale(2.0f, 3.0f);
    actor.SetRotation(45.0f);
    
    Transform2D transform = actor.GetTransform();
    
    EXPECT_FLOAT_EQ(transform.Position.x, 100.0f);
    EXPECT_FLOAT_EQ(transform.Position.y, 200.0f);
    EXPECT_FLOAT_EQ(transform.Scale.x, 2.0f);
    EXPECT_FLOAT_EQ(transform.Scale.y, 3.0f);
    EXPECT_FLOAT_EQ(transform.Rotation, 45.0f);
}
