#include <core/PrefabFactory.h>
#include <core/animation/Animation.h>
#include <core/Entity.h>
#include <core/PrefabComponentRegistry.h>
#include <core/PrefabLibrary.h>

std::unique_ptr<Entity> PrefabFactory::CreateEntity(GameContext& _context, const PrefabDefinition& _definition, const PrefabComponentRegistry& _registry)
{
	std::unique_ptr<Entity> _entity(new Entity(_context, _definition.Texture, _definition.Width, _definition.Height));

	if (!_definition.Animations.empty()) {
		std::unique_ptr<AnimationStateMachine> _anim(new AnimationStateMachine());
		for (const auto& _animDef : _definition.Animations) {
			Vec2 _frameSize = _animDef.FrameSize;
			if (_frameSize.x == 0 && _frameSize.y == 0) {
				_frameSize = Vec2(_definition.Width, _definition.Height);
			}
			AnimPtr _animPtr(new Animation(_animDef.Index, _frameSize, _animDef.Frames, _animDef.Loop, _animDef.Priority));
			_anim->AddAnimation(_animDef.Name, std::move(_animPtr));
		}
		_entity->AssignAnimator(std::move(_anim));
	}

	for (const auto& _component : _definition.Components) {
		const auto* _factory = _registry.Find(_component.Type);
		if (_factory) {
			_entity->AttachComponent((*_factory)(*_entity, _context, _component.Params));
		}
	}

	_entity->SetTag(_definition.Tag);
	_entity->SetPosition(_definition.Position);
	return _entity;
}
