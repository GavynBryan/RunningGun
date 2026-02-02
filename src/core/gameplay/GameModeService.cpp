#include <core/gameplay/GameModeService.h>
#include <core/gameplay/GameMode.h>

GameModeService::~GameModeService()
{
    if (CurrentMode) {
        CurrentMode->Shutdown();
    }
}

void GameModeService::SetGameMode(std::unique_ptr<GameMode> mode)
{
    CurrentMode = std::move(mode);
}

void GameModeService::SwitchGameMode(std::unique_ptr<GameMode> newMode)
{
    if (CurrentMode) {
        CurrentMode->Shutdown();
    }
    SetGameMode(std::move(newMode));
    if (CurrentMode) {
        CurrentMode->Init();
        CurrentMode->BuildScene();
    }
}
