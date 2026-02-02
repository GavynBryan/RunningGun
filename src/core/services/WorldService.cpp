#include <core/services/WorldService.h>
#include <core/GameMode.h>
#include <core/Camera.h>
#include <core/services/RenderService.h>
#include <core/services/TimeService.h>
#include <core/services/ObjectPoolService.h>
#include <algorithm>
#include <cassert>

WorldService::WorldService()
    : Services(GetHost()) 
{
}

WorldService::~WorldService() = default;

void WorldService::Init()
{
    if (SceneInitialized) return;

    BuildScene();
    SceneInitialized = true;
}

void WorldService::Update()
{
    if (!SceneInitialized) {
        Init();
    }

    Start();

    UpdateWorld();
}

void WorldService::AddObject(std::unique_ptr<Entity> entity)
{
    AddQueue.push_back(std::move(entity));
}

void WorldService::ClearEntities()
{
    // Clear object pools first to prevent dangling pointers
    Services.Get<ObjectPoolService>().ClearPools();
    Entities.clear();
    AddQueue.clear();
}

void WorldService::ResetScene()
{
    SceneInitialized = false;
    Services.Get<TimeService>().ResetTimers();
    Services.Get<TimeService>().ResetClock();
    ClearEntities();
}

void WorldService::SetBackgroundTexture(SDL_Texture* texture)
{
    Background.SetTexture(texture);
}

void WorldService::Start()
{
    for (auto& entity : AddQueue) {
        entity->Start();
    }
}

void WorldService::UpdateWorld()
{
    HandleQueue();

    for (auto& entity : Entities) {
        entity->Update();
    }
}

void WorldService::HandleQueue()
{
    for (auto& entity : AddQueue) {
        Entities.push_back(std::move(entity));
    }
    AddQueue.clear();
}