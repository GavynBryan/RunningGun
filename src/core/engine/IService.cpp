#include <core/engine/IService.h>
#include <core/engine/GameServiceHost.h>
#include <cassert>

GameServiceHost& IService::GetHost() const
{
	assert(Host);
	return *Host;
}
