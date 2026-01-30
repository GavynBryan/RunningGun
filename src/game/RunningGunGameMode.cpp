#include <game/RunningGunGameMode.h>
#include <core/ObjectPool.h>
#include <core/PrefabSystem.h>
#include <core/UI/UIManager.h>
#include <core/World.h>
#include <game/components/PlayerComponent.h>
#include <cassert>

RunningGunGameMode::RunningGunGameMode(SDL_Renderer* _renderer, EngineServices& _services, PrefabSystem& _prefabs, World& _world)
	:Renderer(_renderer),
	Services(_services),
	Prefabs(_prefabs),
	WorldContext(_world),
	HealthBar(nullptr),
	StatusTextUI(nullptr),
	GameFont(nullptr),
	ObjectPoolContext(new ObjectPool(_services)),
	PlayerEntity(nullptr),
	PlayerComponentRef(nullptr),
	SpawnScorpion1Interval(10.0f),
	LastSpawn1Time(0.0f),
	SpawnScorpion2Interval(11.0f),
	LastSpawn2Time(0.0f),
	Win(false),
	ResetRequested(false)
{
}

RunningGunGameMode::~RunningGunGameMode()
{
	if (GameFont) {
		TTF_CloseFont(GameFont);
	}
}

void RunningGunGameMode::Init()
{
	if (!TTF_Init()) {
		SDL_Log("Failed to initialize SDL_ttf: %s", SDL_GetError());
	}

	auto& _handler = Services.GetTextureHandler();
	_handler.Load("sprites/ball.png");
	_handler.Load("sprites/bullet.png");
	_handler.Load("sprites/waves.png");
	_handler.Load("sprites/duststorm.png");
	_handler.Load("sprites/background.png");
	_handler.Load("sprites/health.png");

	GameFont = TTF_OpenFont("arial.ttf", 60);
	if (!GameFont) {
		SDL_Log("Failed to load font: %s", SDL_GetError());
	}

	WorldContext.SetBackgroundTexture(_handler.Get("sprites/background.png"));
}

void RunningGunGameMode::BuildScene()
{
	auto& _handler = Services.GetTextureHandler();
	auto _player = Prefabs.Instantiate("player");
	assert(_player);
	PlayerEntity = _player.get();
	PlayerComponentRef = PlayerEntity->GetComponent<PlayerComponent>();
	Services.Instantiate(std::move(_player));
	WorldContext.SetCameraTarget(PlayerEntity);

	auto _bull = Prefabs.Instantiate("bull");
	assert(_bull);
	Services.Instantiate(std::move(_bull));

	for (int _index = 0; _index < 3; _index++) {
		auto _scorpion = Prefabs.Instantiate("scorpion");
		assert(_scorpion);
		ObjectPoolContext->FeedObject(std::move(_scorpion));
	}

	LastSpawn1Time = 0.0f;
	LastSpawn2Time = 0.0f;
	WorldContext.ResetElapsedTime();
	Win = false;

	if (UIManager* _ui = WorldContext.GetUI()) {
		_ui->Clear();
		HealthBar = _ui->AddElement<UIHealthBar>(_handler->Get("sprites/health.png"), 5);
		HealthBar->SetPosition(5, 5);
		HealthBar->SetAnchor(UIAnchor::TopLeft);

		StatusTextUI = _ui->AddElement<UIText>(Renderer, GameFont);
		StatusTextUI->SetPosition(0, 0);
		StatusTextUI->SetAnchor(UIAnchor::Center);
		StatusTextUI->SetVisible(false);

		if (PlayerComponentRef) {
			HealthBar->SetHealthGetter([this]() { return PlayerComponentRef->GetHealth(); });
		}
		SetStatusText("");
	}
}

void RunningGunGameMode::Update()
{
}

void RunningGunGameMode::PostUpdate()
{
	if (!Win) {
		if (Services.GetElapsedTime() - LastSpawn1Time > SpawnScorpion1Interval) {
			auto _scorpion1 = ObjectPoolContext->BorrowObject();
			if (_scorpion1 != nullptr) {
				_scorpion1->SetPosition(50, 20);
				_scorpion1->SetDirection(1, 0);
			}
			LastSpawn1Time = Services.GetElapsedTime();
		}

		if (Services.GetElapsedTime() - LastSpawn2Time > SpawnScorpion2Interval) {
			auto _scorpion2 = ObjectPoolContext->BorrowObject();
			if (_scorpion2 != nullptr) {
				_scorpion2->SetPosition(400, 20);
				_scorpion2->SetDirection(-1, 0);
			}
			LastSpawn2Time = Services.GetElapsedTime();
		}
	}

	if (ResetRequested) {
		WorldContext.ClearEntities();
		ObjectPoolContext->Clear();
		ResetRequested = false;
		BuildScene();
	}
}

void RunningGunGameMode::OnWin()
{
	Win = true;
	if (PlayerEntity) {
		PlayerEntity->Disable();
	}
	SetStatusText("You Win!");
}

void RunningGunGameMode::RequestReset()
{
	ResetRequested = true;
}

void RunningGunGameMode::SetStatusText(const std::string& _text)
{
	if (StatusTextUI) {
		StatusTextUI->SetText(_text);
		StatusTextUI->SetVisible(!_text.empty());
	}
}
