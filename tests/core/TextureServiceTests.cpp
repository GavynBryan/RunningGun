#include <gtest/gtest.h>
#include <core/engine/rendering/IGraphicsAPI.h>
#include <core/engine/rendering/TextureHandle.h>
#include <core/services/rendering/TextureService.h>
#include <core/services/framework/GameServiceHost.h>
#include <core/services/logging/LoggingService.h>
#include <unordered_map>
#include <unordered_set>

// ============= TextureHandle Tests =============

TEST(TextureHandleTest, DefaultConstruction) {
    TextureHandle handle;
    
    EXPECT_EQ(handle.Id, 0u);
    EXPECT_FALSE(handle.IsValid());
}

TEST(TextureHandleTest, InvalidHandle) {
    TextureHandle invalid = TextureHandle::Invalid();
    
    EXPECT_EQ(invalid.Id, 0u);
    EXPECT_FALSE(invalid.IsValid());
}

TEST(TextureHandleTest, ValidHandle) {
    TextureHandle handle{42};
    
    EXPECT_EQ(handle.Id, 42u);
    EXPECT_TRUE(handle.IsValid());
}

TEST(TextureHandleTest, Equality) {
    TextureHandle a{10};
    TextureHandle b{10};
    TextureHandle c{20};
    
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a == c);
    EXPECT_TRUE(a != c);
    EXPECT_FALSE(a != b);
}

TEST(TextureHandleTest, HashableForUnorderedContainers) {
    std::unordered_set<TextureHandle> handles;
    
    handles.insert(TextureHandle{1});
    handles.insert(TextureHandle{2});
    handles.insert(TextureHandle{3});
    handles.insert(TextureHandle{1}); // Duplicate
    
    EXPECT_EQ(handles.size(), 3u);
    EXPECT_TRUE(handles.count(TextureHandle{1}) > 0);
    EXPECT_TRUE(handles.count(TextureHandle{2}) > 0);
    EXPECT_TRUE(handles.count(TextureHandle{3}) > 0);
    EXPECT_FALSE(handles.count(TextureHandle{4}) > 0);
}

// ============= Mock IGraphicsAPI =============

// Mock implementation of IGraphicsAPI for testing
class MockGraphicsAPI : public IGraphicsAPI
{
public:
    // Track calls for verification
    std::unordered_set<uint32_t> UploadedTextures;
    std::unordered_set<uint32_t> DestroyedTextures;
    std::unordered_map<uint32_t, std::string> TexturePaths;
    std::unordered_map<uint32_t, Vec2> TextureSizes;
    bool FailNextUpload = false;

    bool Init() override { return true; }
    void Shutdown() override {}
    void BeginFrame() override {}
    void EndFrame() override {}
    void Clear(const Color&) override {}
    void Present() override {}

    bool UploadTexture(TextureHandle handle, const TextureData&) override {
        if (FailNextUpload) {
            FailNextUpload = false;
            return false;
        }
        UploadedTextures.insert(handle.Id);
        TextureSizes[handle.Id] = Vec2(64.0f, 64.0f);
        return true;
    }

    bool UploadTextureFromFile(TextureHandle handle, const std::string& path) override {
        if (FailNextUpload) {
            FailNextUpload = false;
            return false;
        }
        UploadedTextures.insert(handle.Id);
        TexturePaths[handle.Id] = path;
        TextureSizes[handle.Id] = Vec2(64.0f, 64.0f);
        return true;
    }

    void DestroyTexture(TextureHandle handle) override {
        DestroyedTextures.insert(handle.Id);
        UploadedTextures.erase(handle.Id);
    }

    Vec2 GetTextureSize(TextureHandle handle) const override {
        auto it = TextureSizes.find(handle.Id);
        if (it != TextureSizes.end()) {
            return it->second;
        }
        return Vec2(0.0f, 0.0f);
    }

    void DrawSprite(TextureHandle, const SpriteRenderParams&) override {}
    void DrawRect(const Rectf&, const Color&, bool) override {}
    void DrawLine(const Vec2&, const Vec2&, const Color&) override {}
    void SetCamera(const CameraData&) override {}
    void ResetCamera() override {}
    const char* GetName() const override { return "MockGraphicsAPI"; }
};

class TextureServiceTest : public ::testing::Test
{
protected:
    void SetUp() override {
        Services.AddService<LoggingService>();
        TextureService& texService = Services.AddService<TextureService>(Services);
        texService.SetGraphics(&MockGraphics);
    }

    GameServiceHost Services;
    MockGraphicsAPI MockGraphics;
};

TEST_F(TextureServiceTest, LoadTexture) {
    TextureService& texService = Services.Get<TextureService>();
    
    TextureHandle handle = texService.Load("sprites/player.png");
    
    EXPECT_TRUE(handle.IsValid());
    EXPECT_TRUE(MockGraphics.UploadedTextures.count(handle.Id) > 0);
    EXPECT_EQ(MockGraphics.TexturePaths[handle.Id], "sprites/player.png");
}

TEST_F(TextureServiceTest, LoadSameTextureTwiceReturnsSameHandle) {
    TextureService& texService = Services.Get<TextureService>();
    
    TextureHandle handle1 = texService.Load("sprites/player.png");
    TextureHandle handle2 = texService.Load("sprites/player.png");
    
    EXPECT_EQ(handle1.Id, handle2.Id);
    // Should only upload once
    EXPECT_EQ(MockGraphics.UploadedTextures.size(), 1u);
}

TEST_F(TextureServiceTest, LoadDifferentTexturesReturnsDifferentHandles) {
    TextureService& texService = Services.Get<TextureService>();
    
    TextureHandle handle1 = texService.Load("sprites/player.png");
    TextureHandle handle2 = texService.Load("sprites/enemy.png");
    
    EXPECT_NE(handle1.Id, handle2.Id);
    EXPECT_EQ(MockGraphics.UploadedTextures.size(), 2u);
}

TEST_F(TextureServiceTest, GetReturnsHandleForLoadedTexture) {
    TextureService& texService = Services.Get<TextureService>();
    
    TextureHandle loadedHandle = texService.Load("sprites/player.png");
    TextureHandle gotHandle = texService.Get("sprites/player.png");
    
    EXPECT_EQ(loadedHandle.Id, gotHandle.Id);
}

TEST_F(TextureServiceTest, GetReturnsInvalidForUnloadedTexture) {
    TextureService& texService = Services.Get<TextureService>();
    
    TextureHandle handle = texService.Get("nonexistent.png");
    
    EXPECT_FALSE(handle.IsValid());
}

TEST_F(TextureServiceTest, IsLoaded) {
    TextureService& texService = Services.Get<TextureService>();
    
    EXPECT_FALSE(texService.IsLoaded("sprites/player.png"));
    
    texService.Load("sprites/player.png");
    
    EXPECT_TRUE(texService.IsLoaded("sprites/player.png"));
}

TEST_F(TextureServiceTest, GetPath) {
    TextureService& texService = Services.Get<TextureService>();
    
    TextureHandle handle = texService.Load("sprites/player.png");
    const std::string* path = texService.GetPath(handle);
    
    ASSERT_NE(path, nullptr);
    EXPECT_EQ(*path, "sprites/player.png");
}

TEST_F(TextureServiceTest, GetPathReturnsNullForInvalidHandle) {
    TextureService& texService = Services.Get<TextureService>();
    
    const std::string* path = texService.GetPath(TextureHandle::Invalid());
    
    EXPECT_EQ(path, nullptr);
}

TEST_F(TextureServiceTest, Unload) {
    TextureService& texService = Services.Get<TextureService>();
    
    TextureHandle handle = texService.Load("sprites/player.png");
    EXPECT_TRUE(texService.IsLoaded("sprites/player.png"));
    
    texService.Unload(handle);
    
    EXPECT_FALSE(texService.IsLoaded("sprites/player.png"));
    EXPECT_TRUE(MockGraphics.DestroyedTextures.count(handle.Id) > 0);
}

TEST_F(TextureServiceTest, UnloadAll) {
    TextureService& texService = Services.Get<TextureService>();
    
    TextureHandle h1 = texService.Load("sprites/player.png");
    TextureHandle h2 = texService.Load("sprites/enemy.png");
    TextureHandle h3 = texService.Load("sprites/bullet.png");
    
    texService.UnloadAll();
    
    EXPECT_FALSE(texService.IsLoaded("sprites/player.png"));
    EXPECT_FALSE(texService.IsLoaded("sprites/enemy.png"));
    EXPECT_FALSE(texService.IsLoaded("sprites/bullet.png"));
    EXPECT_EQ(MockGraphics.DestroyedTextures.size(), 3u);
}

TEST_F(TextureServiceTest, GetAllPaths) {
    TextureService& texService = Services.Get<TextureService>();
    
    texService.Load("sprites/a.png");
    texService.Load("sprites/b.png");
    texService.Load("sprites/c.png");
    
    std::vector<std::string> paths = texService.GetAllPaths();
    
    EXPECT_EQ(paths.size(), 3u);
    
    // Check all paths are present (order may vary)
    std::unordered_set<std::string> pathSet(paths.begin(), paths.end());
    EXPECT_TRUE(pathSet.count("sprites/a.png") > 0);
    EXPECT_TRUE(pathSet.count("sprites/b.png") > 0);
    EXPECT_TRUE(pathSet.count("sprites/c.png") > 0);
}

TEST_F(TextureServiceTest, LoadFailureReturnsInvalidHandle) {
    TextureService& texService = Services.Get<TextureService>();
    
    MockGraphics.FailNextUpload = true;
    TextureHandle handle = texService.Load("bad/texture.png");
    
    EXPECT_FALSE(handle.IsValid());
    EXPECT_FALSE(texService.IsLoaded("bad/texture.png"));
}

TEST_F(TextureServiceTest, UniqueHandleIds) {
    TextureService& texService = Services.Get<TextureService>();
    
    TextureHandle h1 = texService.Load("a.png");
    TextureHandle h2 = texService.Load("b.png");
    TextureHandle h3 = texService.Load("c.png");
    
    EXPECT_NE(h1.Id, h2.Id);
    EXPECT_NE(h2.Id, h3.Id);
    EXPECT_NE(h1.Id, h3.Id);
}
