#include <gtest/gtest.h>
#include <core/timing/Timer.h>

TEST(TimerTest, Construction) {
    bool fired = false;
    Timer timer(1, 10.0f, [&fired]() { fired = true; });
    
    EXPECT_EQ(timer.GetHandle(), 1u);
    EXPECT_FALSE(fired);
}

TEST(TimerTest, NotExpiredBeforeEndTime) {
    Timer timer(1, 10.0f, []() {});
    
    EXPECT_FALSE(timer.IsExpired(0.0f));
    EXPECT_FALSE(timer.IsExpired(5.0f));
    EXPECT_FALSE(timer.IsExpired(9.99f));
}

TEST(TimerTest, ExpiredAtEndTime) {
    Timer timer(1, 10.0f, []() {});
    
    EXPECT_TRUE(timer.IsExpired(10.0f));
}

TEST(TimerTest, ExpiredAfterEndTime) {
    Timer timer(1, 10.0f, []() {});
    
    EXPECT_TRUE(timer.IsExpired(15.0f));
    EXPECT_TRUE(timer.IsExpired(100.0f));
}

TEST(TimerTest, FireCallsCallback) {
    int callCount = 0;
    Timer timer(1, 10.0f, [&callCount]() { callCount++; });
    
    EXPECT_EQ(callCount, 0);
    
    timer.Fire();
    EXPECT_EQ(callCount, 1);
    
    timer.Fire();
    EXPECT_EQ(callCount, 2);
}

TEST(TimerTest, FireWithNullCallback) {
    Timer timer(1, 10.0f, nullptr);
    
    // Should not crash
    EXPECT_NO_THROW(timer.Fire());
}

TEST(TimerTest, InvalidTimerHandle) {
    EXPECT_EQ(InvalidTimerHandle, 0u);
}

TEST(TimerTest, UniqueHandles) {
    Timer timer1(1, 5.0f, []() {});
    Timer timer2(2, 10.0f, []() {});
    Timer timer3(3, 15.0f, []() {});
    
    EXPECT_NE(timer1.GetHandle(), timer2.GetHandle());
    EXPECT_NE(timer2.GetHandle(), timer3.GetHandle());
    EXPECT_NE(timer1.GetHandle(), timer3.GetHandle());
}

TEST(TimerTest, ZeroEndTime) {
    Timer timer(1, 0.0f, []() {});
    
    EXPECT_TRUE(timer.IsExpired(0.0f));
}

TEST(TimerTest, NegativeCurrentTime) {
    Timer timer(1, 10.0f, []() {});
    
    EXPECT_FALSE(timer.IsExpired(-5.0f));
}
