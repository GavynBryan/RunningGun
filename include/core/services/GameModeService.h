#pragma once

#include <core/services/IService.h>
#include <memory>

class GameMode;
class WorldService;

class GameModeService final : public IService
{
    std::unique_ptr<GameMode> CurrentMode;
    WorldService& World;
public:
    explicit GameModeService(GameServiceHost& host);

    void SetGameMode(std::unique_ptr<GameMode> mode);
    GameMode* GetCurrentMode() const { return CurrentMode.get(); }
    void SwitchGameMode(std::unique_ptr<GameMode> newMode);
}