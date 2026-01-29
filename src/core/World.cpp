#include <core/World.h>
#include <core/Prefabs.h>
#include <core/AnimationListener.h>
#include <core/ObjectPool.h>
#include <core/Camera.h>
#include <PlayerComponent.h>
#include <algorithm>

World::World(SDL_Renderer* renderer, GameContext& _context)
	:Renderer(renderer),
	Context(_context),
	ResetFlag(false),
	ObjectPoolContext(new ObjectPool(_context)),
	SpawnScorpion1Interval(10.0f),
	LastSpawn1Time(0),
	SpawnScorpion2Interval(11.0f),
	LastSpawn2Time(0),
	Win(false),
	GameFont(nullptr),
	UI(new UIManager(800.0f, 600.0f)),
	HealthBar(nullptr),
	StatusTextUI(nullptr),
	PlayerEntity(nullptr),
	PlayerComponentRef(nullptr),
	GameStartTime(0),
	CollisionTree(Rectf(0.0f, 0.0f, 800.0f, 600.0f)),
	NextTimerHandle(1)
{
}

World::~World()
{
	if (GameFont) {
		TTF_CloseFont(GameFont);
	}
}

float World::GetElapsedTime()
{
	return (SDL_GetTicks() - GameStartTime) / 1000.0f;
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

void World::SetStatusText(const std::string& _text)
{
	if (StatusTextUI) {
		StatusTextUI->SetText(_text);
		StatusTextUI->SetVisible(!_text.empty());
	}
}

void World::UpdateCamera()
{
	Camera* _camera = Context.GetCamera();
	if (_camera && PlayerEntity && PlayerEntity->IsEnabled()) {
		Vec2 _playerPos = PlayerEntity->GetPosition();
		_camera->SetTarget(_playerPos + Vec2(32, 32));
		_camera->Update(Context.DeltaTime());
	}
}

void World::WinGame()
{
	Win = true;
	PlayerEntity->Disable();
	SetStatusText("You Win!");
}

void World::Init()
{
	GameStartTime = SDL_GetTicks();

	// Initialize SDL_ttf
	if (!TTF_Init()) {
		SDL_Log("Failed to initialize SDL_ttf: %s", SDL_GetError());
	}

	auto _handler = Context.GetTextureHandler();
	_handler->Load("sprites/player.png");
	_handler->Load("sprites/ball.png");
	_handler->Load("sprites/bullet.png");
	_handler->Load("sprites/bull.png");
	_handler->Load("sprites/waves.png");
	_handler->Load("sprites/scorpion.png");
	_handler->Load("sprites/duststorm.png");
	_handler->Load("sprites/background.png");
	_handler->Load("sprites/health.png");

	GameFont = TTF_OpenFont("arial.ttf", 60);
	if (!GameFont) {
		SDL_Log("Failed to load font: %s", SDL_GetError());
	}

	Background.SetTexture(_handler->Get("sprites/background.png"));

	// Set up UI
	HealthBar = UI->AddElement<UIHealthBar>(_handler->Get("sprites/health.png"), 5);
	HealthBar->SetPosition(5, 5);
	HealthBar->SetAnchor(UIAnchor::TopLeft);

	StatusTextUI = UI->AddElement<UIText>(Renderer, GameFont);
	StatusTextUI->SetPosition(0, 0);
	StatusTextUI->SetAnchor(UIAnchor::Center);
	StatusTextUI->SetVisible(false);
}

void World::BuildScene()
{
	auto _player = Prefabs::GetPlayer(Context);
	PlayerEntity = _player.get();
	PlayerComponentRef = PlayerEntity->GetComponent<PlayerComponent>();
	AddObject(std::move(_player));

	auto _bull = Prefabs::GetBull(Context);
	AddObject(std::move(_bull));

	for (int _index = 0; _index < 3; _index++) {
		auto _scorpion = Prefabs::GetScorpion(Context);
		ObjectPoolContext->FeedObject(std::move(_scorpion));
	}

	LastSpawn1Time = 0;
	LastSpawn2Time = 0;
	GameStartTime = SDL_GetTicks();

	// Set up health bar to track player health
	if (HealthBar && PlayerComponentRef) {
		HealthBar->SetHealthGetter([this]() { return PlayerComponentRef->GetHealth(); });
	}

	SetStatusText("");
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

	UI->Update(Context.DeltaTime());
}

void World::PostUpdate()
{
	for (auto& _entity : Entities) {
		_entity->PostUpdate();
	}

	if (!Win) {
		if (GetElapsedTime() - LastSpawn1Time > SpawnScorpion1Interval) {
			auto _scorpion1 = ObjectPoolContext->BorrowObject();
			if (_scorpion1 != nullptr) {
				_scorpion1->SetPosition(50, 20);
				_scorpion1->SetDirection(1, 0);
			}
			LastSpawn1Time = GetElapsedTime();
		}

		if (GetElapsedTime() - LastSpawn2Time > SpawnScorpion2Interval) {
			auto _scorpion2 = ObjectPoolContext->BorrowObject();
			if (_scorpion2 != nullptr) {
				_scorpion2->SetPosition(400, 20);
				_scorpion2->SetDirection(-1, 0);
			}
			LastSpawn2Time = GetElapsedTime();
		}
	}

	if (ResetFlag) {
		Entities.clear();
		ObjectPoolContext->Clear();
		ResetFlag = false;
		BuildScene();
	}
}

void World::Render()
{
	Camera* _camera = Context.GetCamera();

	// Render world elements with camera transform
	Background.RenderWithCamera(Renderer, _camera);
	for (auto& _entity : Entities) {
		_entity->RenderWithCamera(Renderer, _camera);
	}

	// Render UI (no camera transform)
	UI->Render(Renderer);
}

void World::HandleQueue()
{
	for (auto& _entity : AddQueue) {
		Entities.push_back(std::move(_entity));
	}
	AddQueue.clear();
}
