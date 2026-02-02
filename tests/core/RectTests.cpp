#include <gtest/gtest.h>
#include <core/math/Rect.h>

TEST(RectTest, DefaultConstructor) {
    Rectf r;
    EXPECT_FLOAT_EQ(r.x, 0.0f);
    EXPECT_FLOAT_EQ(r.y, 0.0f);
    EXPECT_FLOAT_EQ(r.width, 0.0f);
    EXPECT_FLOAT_EQ(r.height, 0.0f);
}

TEST(RectTest, ParameterizedConstructor) {
    Rectf r(10.0f, 20.0f, 100.0f, 50.0f);
    EXPECT_FLOAT_EQ(r.x, 10.0f);
    EXPECT_FLOAT_EQ(r.y, 20.0f);
    EXPECT_FLOAT_EQ(r.width, 100.0f);
    EXPECT_FLOAT_EQ(r.height, 50.0f);
}

TEST(RectTest, Edges) {
    Rectf r(10.0f, 20.0f, 100.0f, 50.0f);
    EXPECT_FLOAT_EQ(r.Left(), 10.0f);
    EXPECT_FLOAT_EQ(r.Top(), 20.0f);
    EXPECT_FLOAT_EQ(r.Right(), 110.0f);
    EXPECT_FLOAT_EQ(r.Bottom(), 70.0f);
}

TEST(RectTest, Position) {
    Rectf r(10.0f, 20.0f, 100.0f, 50.0f);
    Vec2 pos = r.Position();
    EXPECT_FLOAT_EQ(pos.x, 10.0f);
    EXPECT_FLOAT_EQ(pos.y, 20.0f);
}

TEST(RectTest, Size) {
    Rectf r(10.0f, 20.0f, 100.0f, 50.0f);
    Vec2 size = r.Size();
    EXPECT_FLOAT_EQ(size.x, 100.0f);
    EXPECT_FLOAT_EQ(size.y, 50.0f);
}

TEST(RectTest, ContainsPoint) {
    Rectf r(0.0f, 0.0f, 100.0f, 100.0f);
    
    // Inside
    EXPECT_TRUE(r.Contains(50.0f, 50.0f));
    EXPECT_TRUE(r.Contains(0.0f, 0.0f));
    EXPECT_TRUE(r.Contains(99.0f, 99.0f));
    
    // Outside
    EXPECT_FALSE(r.Contains(-1.0f, 50.0f));
    EXPECT_FALSE(r.Contains(100.0f, 50.0f));
    EXPECT_FALSE(r.Contains(50.0f, -1.0f));
    EXPECT_FALSE(r.Contains(50.0f, 100.0f));
}

TEST(RectTest, ContainsVec2) {
    Rectf r(0.0f, 0.0f, 100.0f, 100.0f);
    
    EXPECT_TRUE(r.Contains(Vec2(50.0f, 50.0f)));
    EXPECT_FALSE(r.Contains(Vec2(150.0f, 50.0f)));
}

TEST(RectTest, IntersectsOverlapping) {
    Rectf a(0.0f, 0.0f, 100.0f, 100.0f);
    Rectf b(50.0f, 50.0f, 100.0f, 100.0f);
    
    EXPECT_TRUE(a.Intersects(b));
    EXPECT_TRUE(b.Intersects(a));
}

TEST(RectTest, IntersectsContained) {
    Rectf outer(0.0f, 0.0f, 100.0f, 100.0f);
    Rectf inner(25.0f, 25.0f, 50.0f, 50.0f);
    
    EXPECT_TRUE(outer.Intersects(inner));
    EXPECT_TRUE(inner.Intersects(outer));
}

TEST(RectTest, IntersectsNoOverlap) {
    Rectf a(0.0f, 0.0f, 50.0f, 50.0f);
    Rectf b(100.0f, 100.0f, 50.0f, 50.0f);
    
    EXPECT_FALSE(a.Intersects(b));
    EXPECT_FALSE(b.Intersects(a));
}

TEST(RectTest, IntersectsTouching) {
    Rectf a(0.0f, 0.0f, 50.0f, 50.0f);
    Rectf b(50.0f, 0.0f, 50.0f, 50.0f);  // Touching on right edge
    
    // Touching edges should NOT intersect (uses < not <=)
    EXPECT_FALSE(a.Intersects(b));
}

TEST(RectiTest, IntegerRect) {
    Recti r(10, 20, 100, 50);
    EXPECT_EQ(r.Left(), 10);
    EXPECT_EQ(r.Right(), 110);
    EXPECT_TRUE(r.Contains(50, 40));
    EXPECT_FALSE(r.Contains(5, 40));
}
