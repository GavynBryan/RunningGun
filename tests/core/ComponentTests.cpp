#include <gtest/gtest.h>
#include <core/base/Actor.h>
#include <core/components/TransformComponent.h>
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

TEST_F(ActorTest, GetTransformReturnsTransformComponent) {
    Actor actor;
    
    // Actor should auto-create a transform
    auto* transform = actor.GetTransformComponent();
    EXPECT_NE(transform, nullptr);
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
    
    auto transform = std::make_unique<TransformComponent>(actor, Services);
    TransformComponent* rawPtr = transform.get();
    
    actor.AttachComponent(std::move(transform));
    
    TransformComponent* retrieved = actor.GetComponent<TransformComponent>();
    EXPECT_EQ(retrieved, rawPtr);
}

TEST_F(ActorTest, GetComponentReturnsNullWhenNotPresent) {
    Actor actor;
    
    // Assuming no SpriteComponent attached
    auto* sprite = actor.GetSprite();
    EXPECT_EQ(sprite, nullptr);
}

TEST_F(ActorTest, OnComponentsChangedBroadcasts) {
    Actor actor;
    int callCount = 0;
    
    actor.OnComponentsChanged.Subscribe([&callCount]() {
        callCount++;
    });
    
    auto transform = std::make_unique<TransformComponent>(actor, Services);
    actor.AttachComponent(std::move(transform));
    
    EXPECT_EQ(callCount, 1);
}

// ============= TransformComponent Tests =============

class TransformComponentTest : public ::testing::Test
{
protected:
    void SetUp() override {
        Services.AddService<LoggingService>();
        TestActor = std::make_unique<Actor>();
    }

    GameServiceHost Services;
    std::unique_ptr<Actor> TestActor;
};

TEST_F(TransformComponentTest, DefaultValues) {
    TransformComponent transform(*TestActor, Services);
    
    EXPECT_FLOAT_EQ(transform.GetPosition().x, 0.0f);
    EXPECT_FLOAT_EQ(transform.GetPosition().y, 0.0f);
    EXPECT_FLOAT_EQ(transform.GetDirection().x, 1.0f);  // Default facing right
    EXPECT_FLOAT_EQ(transform.GetDirection().y, 0.0f);
    EXPECT_FLOAT_EQ(transform.GetScale().x, 1.0f);
    EXPECT_FLOAT_EQ(transform.GetScale().y, 1.0f);
    EXPECT_FLOAT_EQ(transform.GetRotation(), 0.0f);
}

TEST_F(TransformComponentTest, SetPosition) {
    TransformComponent transform(*TestActor, Services);
    
    transform.SetPosition(100.0f, 200.0f);
    
    EXPECT_FLOAT_EQ(transform.GetPosition().x, 100.0f);
    EXPECT_FLOAT_EQ(transform.GetPosition().y, 200.0f);
}

TEST_F(TransformComponentTest, SetPositionVec2) {
    TransformComponent transform(*TestActor, Services);
    
    transform.SetPosition(Vec2(50.0f, 75.0f));
    
    EXPECT_FLOAT_EQ(transform.GetPosition().x, 50.0f);
    EXPECT_FLOAT_EQ(transform.GetPosition().y, 75.0f);
}

TEST_F(TransformComponentTest, Translate) {
    TransformComponent transform(*TestActor, Services);
    transform.SetPosition(10.0f, 20.0f);
    
    transform.Translate(5.0f, 10.0f);
    
    EXPECT_FLOAT_EQ(transform.GetPosition().x, 15.0f);
    EXPECT_FLOAT_EQ(transform.GetPosition().y, 30.0f);
}

TEST_F(TransformComponentTest, TranslateVec2) {
    TransformComponent transform(*TestActor, Services);
    transform.SetPosition(10.0f, 20.0f);
    
    transform.Translate(Vec2(-5.0f, -10.0f));
    
    EXPECT_FLOAT_EQ(transform.GetPosition().x, 5.0f);
    EXPECT_FLOAT_EQ(transform.GetPosition().y, 10.0f);
}

TEST_F(TransformComponentTest, SetDirection) {
    TransformComponent transform(*TestActor, Services);
    
    transform.SetDirection(-1.0f, 0.0f);
    
    EXPECT_FLOAT_EQ(transform.GetDirection().x, -1.0f);
    EXPECT_FLOAT_EQ(transform.GetDirection().y, 0.0f);
}

TEST_F(TransformComponentTest, SetUniformScale) {
    TransformComponent transform(*TestActor, Services);
    
    transform.SetScale(2.0f);
    
    EXPECT_FLOAT_EQ(transform.GetScale().x, 2.0f);
    EXPECT_FLOAT_EQ(transform.GetScale().y, 2.0f);
}

TEST_F(TransformComponentTest, SetNonUniformScale) {
    TransformComponent transform(*TestActor, Services);
    
    transform.SetScale(2.0f, 3.0f);
    
    EXPECT_FLOAT_EQ(transform.GetScale().x, 2.0f);
    EXPECT_FLOAT_EQ(transform.GetScale().y, 3.0f);
}

TEST_F(TransformComponentTest, SetRotation) {
    TransformComponent transform(*TestActor, Services);
    
    transform.SetRotation(45.0f);
    
    EXPECT_FLOAT_EQ(transform.GetRotation(), 45.0f);
}

TEST_F(TransformComponentTest, Rotate) {
    TransformComponent transform(*TestActor, Services);
    transform.SetRotation(10.0f);
    
    transform.Rotate(15.0f);
    
    EXPECT_FLOAT_EQ(transform.GetRotation(), 25.0f);
}

TEST_F(TransformComponentTest, GetName) {
    TransformComponent transform(*TestActor, Services);
    
    EXPECT_STREQ(transform.GetName(), "Transform");
}
