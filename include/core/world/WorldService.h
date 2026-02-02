#pragma once

#include <core/engine/framework/IService.h>
#include <core/math/Rect.h>
#include <core/rendering/TextureHandle.h>
#include <core/UI/UIManager.h>
#include <memory>

class GameServiceHost;
class Actor;

// World-level state: bounds, background, camera target, UI.
// Actor tracking moved to ActorService.
class WorldService final : public IService
{
public:
    WorldService();
    ~WorldService();

    // Background texture
    void SetBackgroundTexture(TextureHandle texture);
    TextureHandle GetBackgroundTexture() const { return BackgroundTexture; }

    // World bounds for collision systems
    const Rectf& GetWorldBounds() const { return WorldBounds; }
    void SetWorldBounds(const Rectf& bounds) { WorldBounds = bounds; }

    // UI management
    UIManager* GetUI() { return UI.get(); }
    const UIManager* GetUI() const { return UI.get(); }

    // Camera target (follows entity position)
    void SetCameraTarget(Actor* target);
    Actor* GetCameraTarget() const { return CameraTarget; }

    // Update camera to follow target (called by WorldSystem)
    void UpdateCameraTarget();

private:
    GameServiceHost&            Services;
    TextureHandle               BackgroundTexture;
    Rectf                       WorldBounds = Rectf(0.0f, 0.0f, 800.0f, 600.0f);
    std::unique_ptr<UIManager>  UI;
    Actor*                      CameraTarget = nullptr;
};
