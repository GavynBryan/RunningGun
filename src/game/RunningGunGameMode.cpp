#include <game/RunningGunGameMode.h>
#include <core/PrefabSystem.h>
#include <core/UI/UIManager.h>
#include <core/World.h>
#include <core/engine/ObjectPoolService.h>
#include <core/engine/RenderService.h>
#include <core/engine/RunnerService.h>
#include <core/engine/TimerService.h>
#include <game/components/BullComponent.h>
#include <game/components/PlayerComponent.h>
#include <cassert>

RunningGunGameMode::RunningGunGameMode(SDL_Renderer* _renderer, GameServiceHost& _services, PrefabSystem& _prefabs, World& _world)
	:Renderer(_renderer),
	Services(_services),
	Prefabs(_prefabs),
	WorldContext(_world),
	HealthBar(nullptr),
	StatusTextUI(nullptr),
	GameFont(nullptr),
	PlayerEntity(nullptr),
	BullEntity(nullptr),
	PlayerComponentRef(nullptr),
	BullComponentRef(nullptr),
	SpawnScorpion1Interval(10.0f),
	LastSpawn1Time(0.0f),
	SpawnScorpion2Interval(11.0f),
	LastSpawn2Time(0.0f),
	Win(false),
	Lose(false),
	ResetRequested(false),
	PlayerDiedHandle(0),
	BossDiedHandle(0)
{
}

RunningGunGameMode::~RunningGunGameMode()
{
	UnsubscribeFromEvents();
	if (GameFont) {
		TTF_CloseFont(GameFont);
	}
}

void RunningGunGameMode::Init()
{
	if (!TTF_Init()) {
		SDL_Log("Failed to initialize SDL_ttf: %s", SDL_GetError());
	}

	auto& _handler = Services.Get<RenderService>().GetTextureHandler();
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
	auto& _handler = Services.Get<RenderService>().GetTextureHandler();
	auto _player = Prefabs.Instantiate("player");
	assert(_player);
	PlayerEntity = _player.get();
	PlayerComponentRef = PlayerEntity->GetComponent<PlayerComponent>();
	WorldContext.AddObject(std::move(_player));
	WorldContext.SetCameraTarget(PlayerEntity);

	auto _bull = Prefabs.Instantiate("bull");
	assert(_bull);
	BullEntity = _bull.get();
	BullComponentRef = BullEntity->GetComponent<BullComponent>();
	WorldContext.AddObject(std::move(_bull));

	LastSpawn1Time = 0.0f;
	LastSpawn2Time = 0.0f;
	Services.Get<RunnerService>().ResetClock();
	Services.Get<TimerService>().Reset();
	Win = false;
	Lose = false;

	SubscribeToEvents();

	if (UIManager* _ui = WorldContext.GetUI()) {
		_ui->Clear();
		HealthBar = _ui->AddElement<UIHealthBar>(_handler.Get("sprites/health.png"), 5);
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
	if (!Win && !Lose) {
		if (Services.Get<RunnerService>().GetElapsedTime() - LastSpawn1Time > SpawnScorpion1Interval) {
			auto* _scorpion = Services.Get<ObjectPoolService>().FetchPrefab("scorpion");
			if (_scorpion != nullptr) {
				_scorpion->SetPosition(50, 20);
				_scorpion->SetDirection(1, 0);
			}
			LastSpawn1Time = Services.Get<RunnerService>().GetElapsedTime();
		}

		if (Services.Get<RunnerService>().GetElapsedTime() - LastSpawn2Time > SpawnScorpion2Interval) {
			auto* _scorpion = Services.Get<ObjectPoolService>().FetchPrefab("scorpion");
			if (_scorpion != nullptr) {
				_scorpion->SetPosition(400, 20);
				_scorpion->SetDirection(-1, 0);
			}
			LastSpawn2Time = Services.Get<RunnerService>().GetElapsedTime();
		}
	}

	if (ResetRequested) {
		UnsubscribeFromEvents();
		WorldContext.ClearEntities();
		Services.Get<ObjectPoolService>().ClearPools();
		ResetRequested = false;
		BuildScene();
	}
}

void RunningGunGameMode::OnWin(Entity* _boss)
{
	Win = true;
	if (PlayerEntity) {
		PlayerEntity->Disable();
	}
	SetStatusText("You Win!");
}

void RunningGunGameMode::OnLose(Entity* _player)
{
	Lose = true;
	SetStatusText("Game Over");

	// Schedule reset after delay
	Services.Get<TimerService>().ScheduleTimer(3.0f, [this]() {
		RequestReset();
	});
}

void RunningGunGameMode::RequestReset()
{
	ResetRequested = true;
}

void RunningGunGameMode::SubscribeToEvents()
{
	if (PlayerComponentRef) {
		PlayerDiedHandle = PlayerComponentRef->OnDied.Subscribe([this](Entity* _player) {
			OnLose(_player);
		});
	}

	if (BullComponentRef) {
		BossDiedHandle = BullComponentRef->OnDied.Subscribe([this](Entity* _boss) {
			OnWin(_boss);
		});
	}
}

void RunningGunGameMode::UnsubscribeFromEvents()
{
	if (PlayerComponentRef && PlayerDiedHandle != 0) {
		PlayerComponentRef->OnDied.Unsubscribe(PlayerDiedHandle);
		PlayerDiedHandle = 0;
	}

	if (BullComponentRef && BossDiedHandle != 0) {
		BullComponentRef->OnDied.Unsubscribe(BossDiedHandle);
		BossDiedHandle = 0;
	}
}

void RunningGunGameMode::SetStatusText(const std::string& _text)
{
	if (StatusTextUI) {
		StatusTextUI->SetText(_text);
		StatusTextUI->SetVisible(!_text.empty());
	}
}
