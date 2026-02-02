#include <gtest/gtest.h>
#include <core/engine/rendering/RenderTypes.h>

// ============= Color Tests =============

TEST(ColorTest, DefaultConstruction) {
    Color c;
    
    EXPECT_EQ(c.r, 255);
    EXPECT_EQ(c.g, 255);
    EXPECT_EQ(c.b, 255);
    EXPECT_EQ(c.a, 255);
}

TEST(ColorTest, ParameterizedConstruction) {
    Color c(128, 64, 32, 200);
    
    EXPECT_EQ(c.r, 128);
    EXPECT_EQ(c.g, 64);
    EXPECT_EQ(c.b, 32);
    EXPECT_EQ(c.a, 200);
}

TEST(ColorTest, ConstructionWithDefaultAlpha) {
    Color c(100, 150, 200);
    
    EXPECT_EQ(c.r, 100);
    EXPECT_EQ(c.g, 150);
    EXPECT_EQ(c.b, 200);
    EXPECT_EQ(c.a, 255);
}

TEST(ColorTest, StaticColors) {
    Color white = Color::White();
    EXPECT_EQ(white.r, 255);
    EXPECT_EQ(white.g, 255);
    EXPECT_EQ(white.b, 255);
    
    Color black = Color::Black();
    EXPECT_EQ(black.r, 0);
    EXPECT_EQ(black.g, 0);
    EXPECT_EQ(black.b, 0);
    
    Color red = Color::Red();
    EXPECT_EQ(red.r, 255);
    EXPECT_EQ(red.g, 0);
    EXPECT_EQ(red.b, 0);
    
    Color green = Color::Green();
    EXPECT_EQ(green.r, 0);
    EXPECT_EQ(green.g, 255);
    EXPECT_EQ(green.b, 0);
    
    Color blue = Color::Blue();
    EXPECT_EQ(blue.r, 0);
    EXPECT_EQ(blue.g, 0);
    EXPECT_EQ(blue.b, 255);
    
    Color transparent = Color::Transparent();
    EXPECT_EQ(transparent.a, 0);
}

// ============= SpriteRenderParams Tests =============

TEST(SpriteRenderParamsTest, DefaultValues) {
    SpriteRenderParams params;
    
    EXPECT_FALSE(params.FlipX);
    EXPECT_FALSE(params.FlipY);
    EXPECT_FLOAT_EQ(params.Rotation, 0.0f);
    EXPECT_FLOAT_EQ(params.Origin.x, 0.0f);
    EXPECT_FLOAT_EQ(params.Origin.y, 0.0f);
}

// ============= CameraData Tests =============

TEST(CameraDataTest, DefaultValues) {
    CameraData camera;
    
    EXPECT_FLOAT_EQ(camera.Position.x, 0.0f);
    EXPECT_FLOAT_EQ(camera.Position.y, 0.0f);
    EXPECT_FLOAT_EQ(camera.Rotation, 0.0f);
    EXPECT_FLOAT_EQ(camera.Zoom, 1.0f);
}

TEST(CameraDataTest, CustomValues) {
    CameraData camera;
    camera.Position = Vec2(100.0f, 200.0f);
    camera.Rotation = 45.0f;
    camera.ViewSize = Vec2(800.0f, 600.0f);
    camera.Zoom = 2.0f;
    
    EXPECT_FLOAT_EQ(camera.Position.x, 100.0f);
    EXPECT_FLOAT_EQ(camera.Position.y, 200.0f);
    EXPECT_FLOAT_EQ(camera.Rotation, 45.0f);
    EXPECT_FLOAT_EQ(camera.ViewSize.x, 800.0f);
    EXPECT_FLOAT_EQ(camera.ViewSize.y, 600.0f);
    EXPECT_FLOAT_EQ(camera.Zoom, 2.0f);
}

// ============= TextureData Tests =============

TEST(TextureDataTest, DefaultValues) {
    TextureData data;
    
    EXPECT_TRUE(data.Pixels.empty());
    EXPECT_EQ(data.Width, 0);
    EXPECT_EQ(data.Height, 0);
    EXPECT_EQ(data.Channels, 4);
}

TEST(TextureDataTest, CustomValues) {
    TextureData data;
    data.Width = 64;
    data.Height = 64;
    data.Channels = 4;
    data.Pixels.resize(64 * 64 * 4, 255);
    
    EXPECT_EQ(data.Pixels.size(), 64u * 64u * 4u);
    EXPECT_EQ(data.Width, 64);
    EXPECT_EQ(data.Height, 64);
}
