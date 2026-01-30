#include <core/PrefabLoader.h>
#include <core/Json.h>
#include <core/PrefabLibrary.h>
#include <SDL3/SDL.h>

namespace {
	bool TryParseVec2(simdjson::dom::element _value, Vec2& _out) {
		if (_value.type() != simdjson::dom::element_type::ARRAY) {
			return false;
		}
		auto _arr = _value.get_array().value();
		if (_arr.size() != 2) {
			return false;
		}
		auto _it = _arr.begin();
		auto _x = (*_it).get_double();
		++_it;
		auto _y = (*_it).get_double();
		if (_x.error() || _y.error()) {
			return false;
		}
		_out = Vec2(static_cast<float>(_x.value()), static_cast<float>(_y.value()));
		return true;
	}

	ENTITY_TAG ParseTag(std::string_view _value) {
		if (_value == "player") return player;
		if (_value == "bullet") return bullet;
		if (_value == "enemy_bullet") return enemy_bullet;
		if (_value == "hazard") return hazard;
		if (_value == "pickup") return pickup;
		return player;
	}

	void ParseAnimations(simdjson::dom::element _prefab, PrefabDefinition& _definition) {
		auto _animations = _prefab["animations"].get_array();
		if (_animations.error()) {
			return;
		}
		for (auto _anim : _animations.value()) {
			if (_anim.type() != simdjson::dom::element_type::OBJECT) {
				continue;
			}
			AnimationDefinition _animDef;

			auto _name = _anim["name"].get_string();
			if (!_name.error()) {
				_animDef.Name = std::string(_name.value());
			}

			auto _index = _anim["index"].get_int64();
			if (!_index.error()) {
				_animDef.Index = static_cast<int>(_index.value());
			}

			auto _frames = _anim["frames"].get_int64();
			if (!_frames.error()) {
				_animDef.Frames = static_cast<int>(_frames.value());
			}

			auto _loop = _anim["loop"].get_bool();
			if (!_loop.error()) {
				_animDef.Loop = _loop.value();
			}

			auto _priority = _anim["priority"].get_bool();
			if (!_priority.error()) {
				_animDef.Priority = _priority.value();
			}

			auto _frameSize = _anim["frameSize"];
			if (!_frameSize.error()) {
				TryParseVec2(_frameSize.value(), _animDef.FrameSize);
			}

			if (!_animDef.Name.empty()) {
				_definition.Animations.push_back(std::move(_animDef));
			}
		}
	}

	void ParseComponents(simdjson::dom::element _prefab, PrefabDefinition& _definition) {
		auto _components = _prefab["components"].get_array();
		if (_components.error()) {
			return;
		}

		for (auto _comp : _components.value()) {
			if (_comp.type() != simdjson::dom::element_type::OBJECT) {
				continue;
			}
			ComponentDefinition _compDef;

			auto _type = _comp["type"].get_string();
			if (!_type.error()) {
				_compDef.Type = std::string(_type.value());
			}

			auto _params = _comp["params"];
			if (!_params.error() && _params.value().type() == simdjson::dom::element_type::OBJECT) {
				_compDef.ParamsJson = simdjson::to_string(_params.value());
			}

			if (!_compDef.Type.empty()) {
				_definition.Components.push_back(std::move(_compDef));
			}
		}
	}
}

bool PrefabLoader::LoadFromFile(const std::string& _path, PrefabLibrary& _library)
{
	auto _result = Json::ParseFile(_path);
	if (_result.error()) {
		return false;
	}

	simdjson::dom::element _root = _result.value();
	if (_root.type() != simdjson::dom::element_type::OBJECT) {
		return false;
	}

	auto _prefabs = _root["prefabs"].get_array();
	if (_prefabs.error()) {
		return false;
	}

	_library.Clear();

	for (auto _prefab : _prefabs.value()) {
		if (_prefab.type() != simdjson::dom::element_type::OBJECT) {
			continue;
		}

		PrefabDefinition _definition;

		auto _id = _prefab["id"].get_string();
		if (!_id.error()) {
			_definition.Id = std::string(_id.value());
		}

		auto _texture = _prefab["texture"].get_string();
		if (!_texture.error()) {
			_definition.Texture = std::string(_texture.value());
		}

		auto _width = _prefab["width"].get_double();
		if (!_width.error()) {
			_definition.Width = static_cast<float>(_width.value());
		}

		auto _height = _prefab["height"].get_double();
		if (!_height.error()) {
			_definition.Height = static_cast<float>(_height.value());
		}

		auto _position = _prefab["position"];
		if (!_position.error()) {
			TryParseVec2(_position.value(), _definition.Position);
		}

		auto _tag = _prefab["tag"].get_string();
		if (!_tag.error()) {
			_definition.Tag = ParseTag(_tag.value());
		}

		ParseAnimations(_prefab, _definition);
		ParseComponents(_prefab, _definition);

		if (!_definition.Id.empty()) {
			_library.Add(std::move(_definition));
		}
	}

	return true;
}
