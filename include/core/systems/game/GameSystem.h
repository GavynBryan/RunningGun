#pragma once

#include <core/framework/ISystem.h>

class GameServiceHost;
class GameModeService;

class GameSystem final : public ISystem
{
public:
	explicit GameSystem(GameServiceHost& services);

	void Init() override;
	void Update() override;

private:
	GameModeService& GameMode;
};
