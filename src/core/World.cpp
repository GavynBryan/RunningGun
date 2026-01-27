#include <set>
#include <core/World.h>
#include <core/Game.h>
#include <core/sPrefabs.h>
#include <core/AnimationListener.h>
#include <core/ObjectPool.h>
#include <PlayerComponent.h>

World::World(SDL_Renderer* renderer)
	:mRenderer(renderer),
	resetFlag(false),
	mObjectPool(new ObjectPool()),
	spawnScorpion1Interval(10.0f),
	lastSpawn1Time(0),
	spawnScorpion2Interval(11.0f),
	lastSpawn2Time(0),
	win(false),
	gameFont(nullptr),
	statusTexture(nullptr),
	statusText(""),
	mPlayer(nullptr),
	mPlayerComponent(nullptr),
	gameStartTime(0)
{
	statusRect = {0, 0, 0, 0};
}

World::~World()
{
	if (statusTexture) {
		SDL_DestroyTexture(statusTexture);
	}
	if (gameFont) {
		TTF_CloseFont(gameFont);
	}
}

float World::getElapsedTime()
{
	return (SDL_GetTicks() - gameStartTime) / 1000.0f;
}

void World::addObject(std::unique_ptr<Entity> _entity)
{
	addQueue.push_back(std::move(_entity));
}

void World::updateStatusText(const std::string& text)
{
	statusText = text;
	if (statusTexture) {
		SDL_DestroyTexture(statusTexture);
		statusTexture = nullptr;
	}

	if (!text.empty() && gameFont) {
		SDL_Color white = {255, 255, 255, 255};
		SDL_Surface* surface = TTF_RenderText_Solid(gameFont, text.c_str(), 0, white);
		if (surface) {
			statusTexture = SDL_CreateTextureFromSurface(mRenderer, surface);
			statusRect.w = static_cast<float>(surface->w);
			statusRect.h = static_cast<float>(surface->h);
			statusRect.x = 400 - statusRect.w / 2;
			statusRect.y = 300 - statusRect.h / 2;
			SDL_DestroySurface(surface);
		}
	}
}

void World::winGame()
{
	win = true;
	mPlayer->disable();
	updateStatusText("You Win!");
}

void World::init()
{
	gameStartTime = SDL_GetTicks();

	// Initialize SDL_ttf
	if (!TTF_Init()) {
		SDL_Log("Failed to initialize SDL_ttf: %s", SDL_GetError());
	}

	auto handler = sEnvironment::Instance().getTextureHandler();
	handler->load("sprites/player.png");
	handler->load("sprites/ball.png");
	handler->load("sprites/bullet.png");
	handler->load("sprites/bull.png");
	handler->load("sprites/waves.png");
	handler->load("sprites/scorpion.png");
	handler->load("sprites/duststorm.png");
	handler->load("sprites/background.png");
	handler->load("sprites/health.png");

	gameFont = TTF_OpenFont("arial.ttf", 60);
	if (!gameFont) {
		SDL_Log("Failed to load font: %s", SDL_GetError());
	}

	mBackground.setTexture(handler->get("sprites/background.png"));

	for (int i = 0; i < 5; i++) {
		std::unique_ptr<Entity> heart(new Entity("sprites/health.png", 32, 32));
		heart->setPosition(32.0f * i + 5, 0);
		mHearts.push_back(std::move(heart));
	}
}

void World::buildScene()
{
	auto _player = sPrefabs::getPlayer();
	mPlayer = _player.get();
	mPlayerComponent = mPlayer->getComponent<PlayerComponent>();
	addObject(std::move(_player));

	auto _bull = sPrefabs::getBull();
	addObject(std::move(_bull));

	for (int i = 0; i < 3; i++) {
		auto _scorpion = sPrefabs::getScorpion();
		mObjectPool->feedObject(std::move(_scorpion));
	}

	lastSpawn1Time = 0;
	lastSpawn2Time = 0;
	gameStartTime = SDL_GetTicks();

	updateStatusText("");
}

void World::handleCollisions(CollisionPair pairs)
{

}

void World::start()
{
	for (auto& e : addQueue) {
		e->start();
	}
}

void World::update()
{
	handleQueue();

	std::set<CollisionPair> collisionPairs;

	for (auto& e : mEntities) {
		e->update();
		for (auto& o : mEntities) {
			if (o == e) continue;
			if (e->isEnabled() && o->collision(e.get())) {
				o->onCollide(*e.get());
			}
		}
	}
}

void World::postUpdate()
{
	for (auto& e : mEntities) {
		e->postUpdate();
	}

	if (!win) {
		if (getElapsedTime() - lastSpawn1Time > spawnScorpion1Interval) {
			auto scorpion1 = mObjectPool->borrowObject();
			if (scorpion1 != nullptr) {
				scorpion1->setPosition(50, 20);
				scorpion1->setDirection(1, 0);
			}
			lastSpawn1Time = getElapsedTime();
		}

		if (getElapsedTime() - lastSpawn2Time > spawnScorpion2Interval) {
			auto scorpion2 = mObjectPool->borrowObject();
			if (scorpion2 != nullptr) {
				scorpion2->setPosition(400, 20);
				scorpion2->setDirection(-1, 0);
			}
			lastSpawn2Time = getElapsedTime();
		}
	}

	if (resetFlag) {
		mEntities.clear();
		mObjectPool->clear();
		resetFlag = false;
		buildScene();
	}
}

void World::render()
{
	mBackground.render(mRenderer);
	for (auto& e : mEntities) {
		e->render(mRenderer);
	}
	if (statusTexture) {
		SDL_RenderTexture(mRenderer, statusTexture, nullptr, &statusRect);
	}
	drawHearts(mPlayerComponent->getHealth());
}

void World::drawHearts(int i)
{
	for(int j = 0; j < i; j++){
		mHearts[j]->render(mRenderer);
	}
}

void World::handleQueue()
{
	for (auto& e : addQueue) {
		mEntities.push_back(std::move(e));
	}
	addQueue.clear();
}
