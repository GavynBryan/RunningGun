#include <set>
#include <core/World.h>
#include <core/Game.h>
#include <core/sPrefabs.h>
#include <core/AnimationListener.h>
#include <core/ObjectPool.h>
#include <PlayerComponent.h>

World::World(sf::RenderWindow& _window)
	:mWindow(_window),
	renderView(mWindow.getDefaultView()),
	resetFlag(false),
	mObjectPool(new ObjectPool()),
	spawnScorpion1Interval(10.0f),
	lastSpawn1Time(0),
	spawnScorpion2Interval(11.0f),
	lastSpawn2Time(0),
	win(false),
	statusText()
{

}

World::~World()
{
}

float World::getElapsedTime()
{
	return gameClock.getElapsedTime().asSeconds();
}

void World::addObject(std::unique_ptr<Entity> _entity)
{
	addQueue.push_back(std::move(_entity));
}

void World::init()
{
	gameClock.restart();
	auto &handler = *sEnvironment::Instance().getTextureHandler();
	handler.load("sprites/player.png");
	handler.load("sprites/ball.png");
	handler.load("sprites/bullet.png");
	handler.load("sprites/bull.png");
	handler.load("sprites/waves.png");
	handler.load("sprites/scorpion.png");
	handler.load("sprites/duststorm.png");
	handler.load("sprites/background.png");
	handler.load("sprites/health.png");

	assert(gameFont.loadFromFile("arial.ttf"));

	mBackground.setTexture(handler.get("sprites/background.png"));

	for (int i = 0; i < 5; i++) {
		std::unique_ptr<Entity> heart(new Entity("sprites/health.png", 32, 32));
		heart->setPosition(32 * i + 5, 0);
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
	gameClock.restart();

	statusText.setString("");
	statusText.setPosition(400, 300);
	statusText.setCharacterSize(60);
	statusText.setFont(gameFont);
	statusText.setStyle(sf::Text::Regular);
}

void World::handleCollisions(CollisionPair pairs)
{

}

//call start on entities in the add queue
void World::start()
{
	for (auto& e : addQueue) {
		e->start();
	}
}

void World::update() 
{
	handleQueue();

	//create a set to pass along to the collision loop
	std::set<CollisionPair> collisionPairs;

	for (auto& e : mEntities) {
		e->update();
		//This loop screwed me over, I will have to rewrite this later
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
	mWindow.draw(mBackground);
	for (auto& e : mEntities) {
		e->render(mWindow);
	}
	mWindow.draw(statusText);
	drawHearts(mPlayerComponent->getHealth());
}

void World::drawHearts(int i)
{
	for(int j = 0; j < i; j++){
		mHearts[j]->render(mWindow);
	}
}

void World::handleQueue()
{
	for (auto& e : addQueue) {
		mEntities.push_back(std::move(e));
	}
	addQueue.clear();
}