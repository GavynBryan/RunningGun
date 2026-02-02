#include <core/services/GameModeService.h>
#include <core/GameMode.h>

GameModeService::GameModeService(GameServiceHost& host) {
    // No init needed; host is set via IService.
}

void GameModeService::SetGameMode(std::unique_ptr<GameMode> mode) {
    CurrentMode = std::move(mode);
    // Optional: Notify other services/systems if needed.
}

void GameModeService::SwitchGameMode(std::unique_ptr<GameMode> newMode) {
    if (CurrentMode) {
        CurrentMode->Shutdown();  // If GameMode gains a Shutdown method.
    }
    SetGameMode(std::move(newMode));
    if (CurrentMode) {
        CurrentMode->Init();
    }
}