#include <core/engine/GameServiceHost.h>
#include <algorithm>

void GameServiceHost::Init()
{
	if (Initialized) {
		return;
	}
	SortServices();
	for (const auto& entry : Services) {
		entry.Service->Init();
	}
	Initialized = true;
}

void GameServiceHost::Update()
{
	for (const auto& entry : Services) {
		entry.Service->Update();
	}
}

void GameServiceHost::Shutdown()
{
	if (!Initialized) {
		return;
	}
	for (auto it = Services.rbegin(); it != Services.rend(); ++it) {
		it->Service->Shutdown();
	}
	Initialized = false;
}

void GameServiceHost::SortServices()
{
	std::stable_sort(Services.begin(), Services.end(), [](const ServiceEntry& lhs, const ServiceEntry& rhs) {
		return lhs.Order < rhs.Order;
	});
}
