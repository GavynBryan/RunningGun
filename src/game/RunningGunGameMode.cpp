#include <game/RunningGunGameMode.h>
#include <core/entity/PrefabService.h>
#include <core/UI/UIManager.h>
#include <core/world/World.h>
#include <core/world/WorldService.h>
#include <core/world/ObjectPoolService.h>
#include <core/rendering/RenderContextService.h>
#include <core/rendering/TextureService.h>
#include <core/rendering/backends/SDL3Renderer.h>
#include <core/timing/TimeService.h>
#include <game/components/BullComponent.h>
#include <game/components/PlayerComponent.h>
#include <cassert>

RunningGunGameMode::RunningGunGameMode(GameServiceHost& _services)
	:GameMode(_services)
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

	auto& textures = Services.Get<TextureService>();
	textures.Load("sprites/ball.png");
	textures.Load("sprites/bullet.png");
	textures.Load("sprites/waves.png");
	textures.Load("sprites/duststorm.png");
	textures.Load("sprites/background.png");
	textures.Load("sprites/health.png");

	GameFont = TTF_OpenFont("arial.ttf", 60);
	if (!GameFont) {
		SDL_Log("Failed to load font: %s", SDL_GetError());
	}

	auto& worldService = Services.Get<WorldService>();
	worldService.SetBackgroundTexture(textures.Get("sprites/background.png"));
}

void RunningGunGameMode::BuildScene()
{
	auto& textures = Services.Get<TextureService>();
	auto& prefabs = Services.Get<PrefabService>();
	auto& worldService = Services.Get<WorldService>();

	auto _player = prefabs.Instantiate("player");
	assert(_player);
	PlayerEntity = _player.get();
	PlayerComponentRef = PlayerEntity->GetComponent<PlayerComponent>();
	worldService.AddObject(std::move(_player));
	worldService.SetCameraTarget(PlayerEntity);

	auto _bull = prefabs.Instantiate("bull");
	assert(_bull);
	BullEntity = _bull.get();
	BullComponentRef = BullEntity->GetComponent<BullComponent>();
	worldService.AddObject(std::move(_bull));

	LastSpawn1Time = 0.0f;
	LastSpawn2Time = 0.0f;
	Services.Get<TimeService>().ResetClock();
	Services.Get<TimeService>().ResetTimers();
	Win = false;
	Lose = false;

	SubscribeToEvents();

	if (UIManager* _ui = worldService.GetUI()) {
		_ui->Clear();
		
		// Get native texture for legacy UI (UIHealthBar still uses SDL_Texture*)
		auto& contextService = Services.Get<RenderContextService>();
		RenderContext* primaryContext = contextService.GetPrimaryContext();
		auto* sdlRenderer = primaryContext ? dynamic_cast<SDL3Renderer*>(primaryContext->Graphics.get()) : nullptr;
		SDL_Texture* healthTex = sdlRenderer ? sdlRenderer->GetNativeTexture(textures.Get("sprites/health.png")) : nullptr;
		
		HealthBar = _ui->AddElement<UIHealthBar>(healthTex, 5);
		HealthBar->SetPosition(5, 5);
		HealthBar->SetAnchor(UIAnchor::TopLeft);

		// Get SDL_Renderer from backend for text rendering
		SDL_Renderer* renderer = sdlRenderer ? sdlRenderer->GetSDLRenderer() : nullptr;
		StatusTextUI = _ui->AddElement<UIText>(renderer, GameFont);
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
	if (!Win && !Lose) {
		if (Services.Get<TimeService>().GetElapsedTime() - LastSpawn1Time > SpawnScorpion1Interval) {
			auto* _scorpion = Services.Get<ObjectPoolService>().FetchPrefab("scorpion");
			if (_scorpion != nullptr) {
				_scorpion->SetPosition(50, 20);
				_scorpion->SetDirection(1, 0);
			}
			LastSpawn1Time = Services.Get<TimeService>().GetElapsedTime();
		}

		if (Services.Get<TimeService>().GetElapsedTime() - LastSpawn2Time > SpawnScorpion2Interval) {
			auto* _scorpion = Services.Get<ObjectPoolService>().FetchPrefab("scorpion");
			if (_scorpion != nullptr) {
				_scorpion->SetPosition(400, 20);
				_scorpion->SetDirection(-1, 0);
			}
			LastSpawn2Time = Services.Get<TimeService>().GetElapsedTime();
		}
	}

	if (ResetRequested) {
		UnsubscribeFromEvents();
		auto& worldService = Services.Get<WorldService>();
		worldService.ClearEntities();
		Services.Get<ObjectPoolService>().ClearPools();
		ResetRequested = false;
		BuildScene();
	}
}

void RunningGunGameMode::OnWin(Actor* _boss)
{
	Win = true;
	if (PlayerEntity) {
		PlayerEntity->Disable();
	}
	SetStatusText("You Win!");
}

void RunningGunGameMode::OnLose(Actor* _player)
{
	Lose = true;
	SetStatusText("Game Over");

	// Schedule reset after delay
	Services.Get<TimeService>().ScheduleTimer(3.0f, [this]() {
		RequestReset();
	});
}

void RunningGunGameMode::RequestReset()
{
	ResetRequested = true;
}

void RunningGunGameMode::Shutdown()
{
	UnsubscribeFromEvents();
}

void RunningGunGameMode::SubscribeToEvents()
{
	if (PlayerComponentRef) {
		PlayerDiedHandle = PlayerComponentRef->OnDied.Subscribe([this](Actor* _player) {
			OnLose(_player);
		});
	}

	if (BullComponentRef) {
		BossDiedHandle = BullComponentRef->OnDied.Subscribe([this](Actor* _boss) {
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
