#include <core/World.h>
#include <core/Game.h>
#include <core/Prefabs.h>
#include <core/AnimationListener.h>
#include <core/ObjectPool.h>
#include <PlayerComponent.h>

World::World(SDL_Renderer* renderer)
	:Renderer(renderer),
	ResetFlag(false),
	ObjectPoolContext(new ObjectPool()),
	SpawnScorpion1Interval(10.0f),
	LastSpawn1Time(0),
	SpawnScorpion2Interval(11.0f),
	LastSpawn2Time(0),
	Win(false),
	GameFont(nullptr),
	StatusTexture(nullptr),
	StatusText(""),
	PlayerEntity(nullptr),
	PlayerComponentRef(nullptr),
	GameStartTime(0),
	CollisionTree(Rectf(0.0f, 0.0f, 800.0f, 600.0f))
{
	StatusRect = {0, 0, 0, 0};
}

World::~World()
{
	if (StatusTexture) {
		SDL_DestroyTexture(StatusTexture);
	}
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

void World::UpdateStatusText(const std::string& _text)
{
	StatusText = _text;
	if (StatusTexture) {
		SDL_DestroyTexture(StatusTexture);
		StatusTexture = nullptr;
	}

	if (!_text.empty() && GameFont) {
		SDL_Color _white = {255, 255, 255, 255};
		SDL_Surface* _surface = TTF_RenderText_Solid(GameFont, _text.c_str(), 0, _white);
		if (_surface) {
			StatusTexture = SDL_CreateTextureFromSurface(Renderer, _surface);
			StatusRect.w = static_cast<float>(_surface->w);
			StatusRect.h = static_cast<float>(_surface->h);
			StatusRect.x = 400 - StatusRect.w / 2;
			StatusRect.y = 300 - StatusRect.h / 2;
			SDL_DestroySurface(_surface);
		}
	}
}

void World::WinGame()
{
	Win = true;
	PlayerEntity->Disable();
	UpdateStatusText("You Win!");
}

void World::Init()
{
	GameStartTime = SDL_GetTicks();

	// Initialize SDL_ttf
	if (!TTF_Init()) {
		SDL_Log("Failed to initialize SDL_ttf: %s", SDL_GetError());
	}

	auto _handler = Environment::Instance().GetTextureHandler();
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

	for (int _index = 0; _index < 5; _index++) {
		std::unique_ptr<Entity> _heart(new Entity("sprites/health.png", 32, 32));
		_heart->SetPosition(32.0f * _index + 5, 0);
		Hearts.push_back(std::move(_heart));
	}
}

void World::BuildScene()
{
	auto _player = Prefabs::GetPlayer();
	PlayerEntity = _player.get();
	PlayerComponentRef = PlayerEntity->GetComponent<PlayerComponent>();
	AddObject(std::move(_player));

	auto _bull = Prefabs::GetBull();
	AddObject(std::move(_bull));

	for (int _index = 0; _index < 3; _index++) {
		auto _scorpion = Prefabs::GetScorpion();
		ObjectPoolContext->FeedObject(std::move(_scorpion));
	}

	LastSpawn1Time = 0;
	LastSpawn2Time = 0;
	GameStartTime = SDL_GetTicks();

	UpdateStatusText("");
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
	Background.Render(Renderer);
	for (auto& _entity : Entities) {
		_entity->Render(Renderer);
	}
	if (StatusTexture) {
		SDL_RenderTexture(Renderer, StatusTexture, nullptr, &StatusRect);
	}
	DrawHearts(PlayerComponentRef->GetHealth());
}

void World::DrawHearts(int _heartCount)
{
	for(int _index = 0; _index < _heartCount; _index++){
		Hearts[_index]->Render(Renderer);
	}
}

void World::HandleQueue()
{
	for (auto& _entity : AddQueue) {
		Entities.push_back(std::move(_entity));
	}
	AddQueue.clear();
}
