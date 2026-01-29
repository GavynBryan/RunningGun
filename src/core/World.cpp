#include <core/World.h>
#include <core/GameMode.h>
#include <core/Camera.h>
#include <algorithm>

World::World(SDL_Renderer* renderer, GameContext& _context)
	:Renderer(renderer),
	Context(_context),
	GameStartTime(0),
	CameraTarget(nullptr),
	CollisionTree(Rectf(0.0f, 0.0f, 800.0f, 600.0f)),
	NextTimerHandle(1),
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

float World::GetElapsedTime()
{
	return (SDL_GetTicks() - GameStartTime) / 1000.0f;
}

void World::ResetElapsedTime()
{
	GameStartTime = SDL_GetTicks();
}

void World::AddObject(std::unique_ptr<Entity> _entity)
{
	AddQueue.push_back(std::move(_entity));
}

TimerHandle World::ScheduleTimer(float _delay, std::function<void()> _callback)
{
	TimerHandle _handle = NextTimerHandle++;
	float _endTime = GetElapsedTime() + _delay;
	Timers.push_back(std::make_unique<Timer>(_handle, _endTime, _callback));
	return _handle;
}

void World::CancelTimer(TimerHandle _handle)
{
	Timers.erase(
		std::remove_if(Timers.begin(), Timers.end(),
			[_handle](const std::unique_ptr<Timer>& _t) { return _t->GetHandle() == _handle; }),
		Timers.end());
}

void World::ClearEntities()
{
	Entities.clear();
	AddQueue.clear();
	Timers.clear();
	NextTimerHandle = 1;
}

void World::Reset()
{
	if (Mode) {
		Mode->RequestReset();
	}
}

void World::UpdateTimers()
{
	float _currentTime = GetElapsedTime();
	for (auto _it = Timers.begin(); _it != Timers.end(); ) {
		if ((*_it)->IsExpired(_currentTime)) {
			(*_it)->Fire();
			_it = Timers.erase(_it);
		} else {
			++_it;
		}
	}
}

void World::UpdateCamera()
{
	Camera* _camera = Context.GetCamera();
	if (_camera && CameraTarget && CameraTarget->IsEnabled()) {
		Vec2 _targetPos = CameraTarget->GetPosition();
		_camera->SetTarget(_targetPos + Vec2(32, 32));
		_camera->Update(Context.DeltaTime());
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
	GameStartTime = SDL_GetTicks();
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
	UpdateTimers();
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
		UI->Update(Context.DeltaTime());
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
	Camera* _camera = Context.GetCamera();

	// Render world elements with camera transform
	Background.Render(Renderer, _camera);
	for (auto& _entity : Entities) {
		_entity->Render(Renderer, _camera);
	}

	if (UI) {
		UI->Render(Renderer);
	}
}

void World::HandleQueue()
{
	for (auto& _entity : AddQueue) {
		Entities.push_back(std::move(_entity));
	}
	AddQueue.clear();
}
