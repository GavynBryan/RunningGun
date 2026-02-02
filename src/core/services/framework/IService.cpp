#include <core/services/framework/IService.h>
#include <core/services/framework/GameServiceHost.h>

GameServiceHost& IService::GetHost() const
{
	return *Host;
}
