#pragma once

class GameServiceHost;

class IService
{
public:
	virtual ~IService() = default;

	virtual void Init() {}
	virtual void Update() {}
	virtual void Shutdown() {}

	void SetHost(GameServiceHost* host) { Host = host; }

protected:
	GameServiceHost& GetHost() const;

private:
	GameServiceHost* Host = nullptr;
};
