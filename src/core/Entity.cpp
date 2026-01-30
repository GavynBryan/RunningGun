#include <core/Entity.h>
#include <core/engine/GameServiceHost.h>
#include <core/engine/RenderService.h>
#include <core/Camera.h>


Entity::Entity(GameServiceHost& _services, std::string _texture, float _width, float _height)
	:Position(0,0),
	Activated(true),
	Services(_services)
{
	//load resource handler from service
	auto& _handler = Services.Get<RenderService>().GetTextureHandler();
	Sprite.SetTexture(_handler.Get(_texture));
	Sprite.SetTextureRect(Recti(0, 0, static_cast<int>(_width), static_cast<int>(_height)));
}


Entity::~Entity()
{
}

void Entity::AttachComponent(std::unique_ptr<Component> _comp)
{
	Components.push_back(std::move(_comp));
}

void Entity::StartComponents()
{
	if (Activated) {
		for (auto& _component : Components) {
			_component->Start();
		}
	}
}

void Entity::UpdateComponents()
{
	if (Activated) {
		for (auto& _component : Components) {
			_component->Update();
		}
	}
}

void Entity::PostUpdateComponents()
{
	if (Activated) {
		for (auto& _component : Components) {
			_component->PostUpdate();
		}
	}
}

void Entity::Start()
{
	StartComponents();
	//so that collision isn't detected on the first frame
	Sprite.SetPosition(Position);
}

void Entity::Update()
{
	if (Activated) {
		UpdateComponents();
	}
}

void Entity::PostUpdate()
{
	if (Activated) {
		PostUpdateComponents();

		if (Animator != nullptr) {
			Animator->Update(Sprite);
		}
	}
}

void Entity::Render(SDL_Renderer* _renderer, Camera* _camera)
{
	if (Activated) {
		Sprite.SetPosition(Position);
		Sprite.Render(_renderer, _camera);
	}
}

void Entity::SetPosition(float _x, float _y)
{
	Position.x = _x; Position.y = _y;
	Sprite.SetPosition(Position);
}

void Entity::SetPosition(Vec2 _pos)
{
	Position = _pos;
	Sprite.SetPosition(Position);
}


void Entity::SetDirection(Vec2 _dir)
{
	Direction = _dir;
}

void Entity::SetDirection(float _x, float _y)
{
	Direction.x = _x; Direction.y = _y;
}

void Entity::AssignAnimator(std::unique_ptr<AnimationStateMachine> _animator)
{
	Animator = std::move(_animator);
}

void Entity::OnCollide(Entity& _other)
{
	if (Activated) {
		for (auto& _component : Components) {
			_component->OnCollide(_other);
		}
	}
}
