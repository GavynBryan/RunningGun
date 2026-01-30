#include <core/World.h>
#include <core/GameMode.h>
#include <core/Camera.h>
#include <core/engine/RenderService.h>
#include <core/engine/RunnerService.h>
#include <algorithm>

World::World(GameServiceHost& _services)
	:Services(_services),
	CameraTarget(nullptr),
	CollisionTree(Rectf(0.0f, 0.0f, 800.0f, 600.0f)),
	Mode(nullptr),
	UI(new UIManager(800.0f, 600.0f))
{
}

World::~World()
{
}

void World::SetGameMode(GameMode* _mode)
{
	Mode = _mode;
}


void World::AddObject(std::unique_ptr<Entity> _entity)
{
	AddQueue.push_back(std::move(_entity));
}

void World::ClearEntities()
{
	Entities.clear();
	AddQueue.clear();
}

void World::Reset()
{
	if (Mode) {
		Mode->RequestReset();
	}
}

void World::UpdateCamera()
{
	Camera* _camera = &Services.Get<RenderService>().GetCamera();
	if (_camera && CameraTarget && CameraTarget->IsEnabled()) {
		Vec2 _targetPos = CameraTarget->GetPosition();
		_camera->SetTarget(_targetPos + Vec2(32, 32));
		_camera->Update(Services.Get<RunnerService>().GetDeltaTime());
	}
}

void World::SetBackgroundTexture(SDL_Texture* _texture)
{
	Background.SetTexture(_texture);
}

void World::SetCameraTarget(Entity* _entity)
{
	CameraTarget = _entity;
}

void World::Init()
{
}

void World::BuildScene()
{
	if (Mode) {
		Mode->BuildScene();
	}
}

void World::Start()
{
	for (auto& _entity : AddQueue) {
		_entity->Start();
	}
}

void World::Update()
{
	HandleQueue();
	UpdateCamera();

	for (auto& _entity : Entities) {
		_entity->Update();
	}

	CollisionTree.Clear();
	for (auto& _entity : Entities) {
		if (_entity->IsEnabled()) {
			CollisionTree.Insert(_entity.get());
		}
	}

	for (auto& _entity : Entities) {
		if (!_entity->IsEnabled()) {
			continue;
		}
		CollisionCandidates.clear();
		CollisionTree.Query(_entity->GetBoundingRect(), CollisionCandidates);
		for (auto* _other : CollisionCandidates) {
			if (_other == _entity.get()) {
				continue;
			}
			if (!_other->IsEnabled()) {
				continue;
			}
			if (_entity.get() < _other && _entity->Collision(_other)) {
				_entity->OnCollide(*_other);
				_other->OnCollide(*_entity.get());
			}
		}
	}

	if (UI) {
		UI->Update(Services.Get<RunnerService>().GetDeltaTime());
	}
}

void World::PostUpdate()
{
	for (auto& _entity : Entities) {
		_entity->PostUpdate();
	}
}

void World::Render()
{
	auto& renderService = Services.Get<RenderService>();
	Camera* _camera = &renderService.GetCamera();
	SDL_Renderer* renderer = renderService.GetRenderer();

	// Render world elements with camera transform
	Background.Render(renderer, _camera);
	for (auto& _entity : Entities) {
		_entity->Render(renderer, _camera);
	}

	if (UI) {
		UI->Render(renderer);
	}
}

void World::HandleQueue()
{
	for (auto& _entity : AddQueue) {
		Entities.push_back(std::move(_entity));
	}
	AddQueue.clear();
}
