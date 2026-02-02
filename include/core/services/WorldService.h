#pragma once

#include <SDL3/SDL.h>
#include <core/Entity.h>
#include <core/Rect.h>
#include <core/services/GameServiceHost.h>
#include <core/services/IService.h>
#include <core/Sprite.h>
#include <core/UI/UIManager.h>
#include <memory>
#include <vector>

class GameMode;
class Camera;

class WorldService final : public IService
{
public:
    WorldService();
    ~WorldService();

    void Init() override;
    void Update() override;

    void AddObject(std::unique_ptr<Entity> entity);
    void ClearEntities();
    void ResetScene();

    void SetBackgroundTexture(SDL_Texture* texture);

    void BuildScene();

    void Start();
    void UpdateWorld();

    const std::vector<Entity::Ptr>& GetEntities() const { return Entities; }

    // World bounds for collision systems
    const Rectf& GetWorldBounds() const { return WorldBounds; }
    void SetWorldBounds(const Rectf& bounds) { WorldBounds = bounds; }

private:
    void HandleQueue();

    GameServiceHost&            Services;          
    Sprite                      Background;
    std::vector<Entity::Ptr>    Entities;
    std::vector<Entity::Ptr>    AddQueue;
    Rectf                       WorldBounds = Rectf(0.0f, 0.0f, 800.0f, 600.0f);

    bool                        SceneInitialized = false;
};