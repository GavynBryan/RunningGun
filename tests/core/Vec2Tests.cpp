#include <gtest/gtest.h>
#include <core/math/Vec2.h>

TEST(Vec2Test, DefaultConstructor) {
    Vec2 v;
    EXPECT_FLOAT_EQ(v.x, 0.0f);
    EXPECT_FLOAT_EQ(v.y, 0.0f);
}

TEST(Vec2Test, ParameterizedConstructor) {
    Vec2 v(3.0f, 4.0f);
    EXPECT_FLOAT_EQ(v.x, 3.0f);
    EXPECT_FLOAT_EQ(v.y, 4.0f);
}

TEST(Vec2Test, Addition) {
    Vec2 a(1.0f, 2.0f);
    Vec2 b(3.0f, 4.0f);
    Vec2 result = a + b;
    EXPECT_FLOAT_EQ(result.x, 4.0f);
    EXPECT_FLOAT_EQ(result.y, 6.0f);
}

TEST(Vec2Test, Subtraction) {
    Vec2 a(5.0f, 7.0f);
    Vec2 b(2.0f, 3.0f);
    Vec2 result = a - b;
    EXPECT_FLOAT_EQ(result.x, 3.0f);
    EXPECT_FLOAT_EQ(result.y, 4.0f);
}

TEST(Vec2Test, ScalarMultiplication) {
    Vec2 v(2.0f, 3.0f);
    Vec2 result = v * 2.0f;
    EXPECT_FLOAT_EQ(result.x, 4.0f);
    EXPECT_FLOAT_EQ(result.y, 6.0f);
}

TEST(Vec2Test, ScalarDivision) {
    Vec2 v(6.0f, 8.0f);
    Vec2 result = v / 2.0f;
    EXPECT_FLOAT_EQ(result.x, 3.0f);
    EXPECT_FLOAT_EQ(result.y, 4.0f);
}

TEST(Vec2Test, Length) {
    Vec2 v(3.0f, 4.0f);
    EXPECT_FLOAT_EQ(v.Length(), 5.0f);
}

TEST(Vec2Test, Normalized) {
    Vec2 v(3.0f, 4.0f);
    Vec2 normalized = v.Normalized();
    EXPECT_FLOAT_EQ(normalized.Length(), 1.0f);
    EXPECT_FLOAT_EQ(normalized.x, 0.6f);
    EXPECT_FLOAT_EQ(normalized.y, 0.8f);
}

TEST(Vec2Test, NormalizedZeroVector) {
    Vec2 v(0.0f, 0.0f);
    Vec2 normalized = v.Normalized();
    EXPECT_FLOAT_EQ(normalized.x, 0.0f);
    EXPECT_FLOAT_EQ(normalized.y, 0.0f);
}

TEST(Vec2Test, Equality) {
    Vec2 a(1.0f, 2.0f);
    Vec2 b(1.0f, 2.0f);
    Vec2 c(1.0f, 3.0f);
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a == c);
}

TEST(Vec2Test, CompoundAddition) {
    Vec2 a(1.0f, 2.0f);
    a += Vec2(3.0f, 4.0f);
    EXPECT_FLOAT_EQ(a.x, 4.0f);
    EXPECT_FLOAT_EQ(a.y, 6.0f);
}
