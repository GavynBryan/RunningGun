#pragma once

#include <memory>
#include <string>
#include <core/engine/GameplayServices.h>
#include <core/engine/FrameClock.h>
#include <core/engine/RenderServices.h>
#include <core/engine/WorldServices.h>
#include <core/events/GameStateEvents.h>

class Camera;
class InputManager;
class Physics;
class ResourceHandler;
class World;

class EngineServices final : public GameplayServices
{
public:
	EngineServices();
	~EngineServices() override = default;

	RenderServices& Render();
	const RenderServices& Render() const;

	ResourceHandler& GetTextureHandler() const;
	Camera& GetCamera() const;
	Physics& GetPhysics() const override;
	InputManager& GetInput() const override;
	GameStateEvents& GetGameStateEvents() override;
	float GetElapsedTime() const override;
	float DeltaTime() const override;

	std::unique_ptr<Entity> CreateEntity(const std::string& _texture, float _width, float _height) override;
	void Instantiate(std::unique_ptr<Entity> _entity) const override;
	TimerHandle ScheduleTimer(float _delay, std::function<void()> _callback) const override;
	void CancelTimer(TimerHandle _handle) const override;
	void Reset() const override;
	GameMode* GetGameMode() const override;

private:
	RenderServices RenderContext;
	std::unique_ptr<Physics> PhysicsManager;
	FrameClock FrameTime;
	WorldServices WorldContext;
	GameStateEvents StateEvents;
	InputManager* InputContext = nullptr;

	void SetTextureHandler(std::unique_ptr<ResourceHandler> _handler);
	void SetPhysics(std::unique_ptr<Physics> _physics);
	void SetCamera(std::unique_ptr<Camera> _camera);
	void SetWorld(World* _world);
	void SetDeltaTime(float _deltaTime);
	void SetInput(InputManager* _input);

	friend class Engine;
};
