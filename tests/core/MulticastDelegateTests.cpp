#include <gtest/gtest.h>
#include <core/events/MulticastDelegate.h>

TEST(MulticastDelegateTest, SubscribeAndBroadcast) {
    MulticastDelegate<int> delegate;
    int receivedValue = 0;
    
    delegate.Subscribe([&receivedValue](int value) {
        receivedValue = value;
    });
    
    delegate.Broadcast(42);
    EXPECT_EQ(receivedValue, 42);
}

TEST(MulticastDelegateTest, MultipleSubscribers) {
    MulticastDelegate<int> delegate;
    int sum = 0;
    
    delegate.Subscribe([&sum](int value) { sum += value; });
    delegate.Subscribe([&sum](int value) { sum += value * 2; });
    delegate.Subscribe([&sum](int value) { sum += value * 3; });
    
    delegate.Broadcast(10);
    EXPECT_EQ(sum, 60);  // 10 + 20 + 30
}

TEST(MulticastDelegateTest, Unsubscribe) {
    MulticastDelegate<int> delegate;
    int callCount = 0;
    
    auto handle = delegate.Subscribe([&callCount](int) { callCount++; });
    
    delegate.Broadcast(1);
    EXPECT_EQ(callCount, 1);
    
    delegate.Unsubscribe(handle);
    delegate.Broadcast(1);
    EXPECT_EQ(callCount, 1);  // Should not have been called again
}

TEST(MulticastDelegateTest, UnsubscribeSpecificHandle) {
    MulticastDelegate<> delegate;
    int countA = 0;
    int countB = 0;
    
    auto handleA = delegate.Subscribe([&countA]() { countA++; });
    delegate.Subscribe([&countB]() { countB++; });
    
    delegate.Broadcast();
    EXPECT_EQ(countA, 1);
    EXPECT_EQ(countB, 1);
    
    delegate.Unsubscribe(handleA);
    delegate.Broadcast();
    EXPECT_EQ(countA, 1);  // A not called
    EXPECT_EQ(countB, 2);  // B still called
}

TEST(MulticastDelegateTest, HasSubscribers) {
    MulticastDelegate<> delegate;
    
    EXPECT_FALSE(delegate.HasSubscribers());
    
    auto handle = delegate.Subscribe([]() {});
    EXPECT_TRUE(delegate.HasSubscribers());
    
    delegate.Unsubscribe(handle);
    EXPECT_FALSE(delegate.HasSubscribers());
}

TEST(MulticastDelegateTest, SubscriberCount) {
    MulticastDelegate<> delegate;
    
    EXPECT_EQ(delegate.SubscriberCount(), 0u);
    
    auto h1 = delegate.Subscribe([]() {});
    EXPECT_EQ(delegate.SubscriberCount(), 1u);
    
    auto h2 = delegate.Subscribe([]() {});
    EXPECT_EQ(delegate.SubscriberCount(), 2u);
    
    delegate.Unsubscribe(h1);
    EXPECT_EQ(delegate.SubscriberCount(), 1u);
}

TEST(MulticastDelegateTest, Clear) {
    MulticastDelegate<> delegate;
    
    delegate.Subscribe([]() {});
    delegate.Subscribe([]() {});
    delegate.Subscribe([]() {});
    
    EXPECT_EQ(delegate.SubscriberCount(), 3u);
    
    delegate.Clear();
    EXPECT_EQ(delegate.SubscriberCount(), 0u);
    EXPECT_FALSE(delegate.HasSubscribers());
}

TEST(MulticastDelegateTest, MultipleArguments) {
    MulticastDelegate<int, float, const std::string&> delegate;
    int receivedInt = 0;
    float receivedFloat = 0.0f;
    std::string receivedString;
    
    delegate.Subscribe([&](int i, float f, const std::string& s) {
        receivedInt = i;
        receivedFloat = f;
        receivedString = s;
    });
    
    delegate.Broadcast(42, 3.14f, std::string("hello"));
    
    EXPECT_EQ(receivedInt, 42);
    EXPECT_FLOAT_EQ(receivedFloat, 3.14f);
    EXPECT_EQ(receivedString, "hello");
}

TEST(MulticastDelegateTest, UniqueHandles) {
    MulticastDelegate<> delegate;
    
    auto h1 = delegate.Subscribe([]() {});
    auto h2 = delegate.Subscribe([]() {});
    auto h3 = delegate.Subscribe([]() {});
    
    EXPECT_NE(h1, h2);
    EXPECT_NE(h2, h3);
    EXPECT_NE(h1, h3);
}

TEST(MulticastDelegateTest, SafeBroadcastDuringModification) {
    MulticastDelegate<> delegate;
    int callCount = 0;
    DelegateHandle handleToRemove = 0;
    
    // Subscriber that unsubscribes another during broadcast
    delegate.Subscribe([&]() {
        callCount++;
        delegate.Unsubscribe(handleToRemove);
    });
    
    handleToRemove = delegate.Subscribe([&]() {
        callCount++;
    });
    
    // Both should be called because we snapshot before iterating
    delegate.Broadcast();
    EXPECT_EQ(callCount, 2);
    
    // Now only one subscriber remains
    EXPECT_EQ(delegate.SubscriberCount(), 1u);
}
