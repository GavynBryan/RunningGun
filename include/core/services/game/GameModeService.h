#pragma once

#include <core/framework/IService.h>
#include <memory>

class GameMode;

// Service that manages the current game mode lifecycle.
// Handles initialization, switching, and shutdown of game modes.
class GameModeService final : public IService
{
public:
    GameModeService() = default;
    ~GameModeService() override;

    void SetGameMode(std::unique_ptr<GameMode> mode);
    GameMode* GetCurrentMode() const { return CurrentMode.get(); }
    void SwitchGameMode(std::unique_ptr<GameMode> newMode);

private:
    std::unique_ptr<GameMode> CurrentMode;
};
