#pragma once

#include <core/framework/IService.h>
#include <game/input/PlayerInputConfig.h>

// Service wrapper for PlayerInputConfig to enable service locator access.
// This allows PlayerComponent to retrieve input configuration without
// requiring it to be passed through the component factory.
class PlayerInputConfigService final : public IService
{
public:
	explicit PlayerInputConfigService(PlayerInputConfig config)
		: m_Config(std::move(config))
	{
	}

	const PlayerInputConfig& GetConfig() const { return m_Config; }
	PlayerInputConfig& GetConfig() { return m_Config; }

private:
	PlayerInputConfig m_Config;
};
