#pragma once

#include <core/gameplay/GameMode.h>
#include <core/events/MulticastDelegate.h>
#include <core/UI/UIHealthBar.h>
#include <core/UI/UIText.h>
#include <memory>
#include <string>

class SDL_Renderer;  // Forward declare if needed elsewhere.
class TTF_Font;
class BullComponent;
class PlayerComponent;
class Actor;

class RunningGunGameMode : public GameMode {
private:
    UIHealthBar* HealthBar = nullptr;
    UIText* StatusTextUI = nullptr;
    TTF_Font* GameFont = nullptr;

    Actor* PlayerEntity = nullptr;
    Actor* BullEntity = nullptr;
    PlayerComponent* PlayerComponentRef = nullptr;
    BullComponent* BullComponentRef = nullptr;

    float SpawnScorpion1Interval = 10.0f;
    float LastSpawn1Time = 0.0f;
    float SpawnScorpion2Interval = 11.0f;
    float LastSpawn2Time = 0.0f;

    bool Win = false;
    bool Lose = false;
    bool ResetRequested = false;

    DelegateHandle PlayerDiedHandle = 0;
    DelegateHandle BossDiedHandle = 0;

    void SetStatusText(const std::string& text);
    void SubscribeToEvents();
    void UnsubscribeFromEvents();

public:
    RunningGunGameMode(GameServiceHost& services);
    ~RunningGunGameMode() override;

    void Init() override;
    void BuildScene() override;
    void Update() override;
    void Shutdown() override;
    void RequestReset() override;

    void OnWin(Entity* boss);
    void OnLose(Entity* player);
};