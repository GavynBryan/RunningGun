#include <core/PrefabLoader.h>
#include <core/JsonParser.h>
#include <core/PrefabLibrary.h>
#include <SDL3/SDL.h>

namespace {
	bool TryParseVec2(const JsonValue& _value, Vec2& _out) {
		if (!_value.IsArray() || _value.Size() != 2) {
			return false;
		}
		if (!_value[0].IsNumber() || !_value[1].IsNumber()) {
			return false;
		}
		_out = Vec2(
			static_cast<float>(_value[0].GetDouble()),
			static_cast<float>(_value[1].GetDouble())
		);
		return true;
	}

	ENTITY_TAG ParseTag(const std::string& _value) {
		if (_value == "player") {
			return player;
		}
		if (_value == "bullet") {
			return bullet;
		}
		if (_value == "enemy_bullet") {
			return enemy_bullet;
		}
		if (_value == "hazard") {
			return hazard;
		}
		if (_value == "pickup") {
			return pickup;
		}
		return player;
	}

	void ParseAnimations(const JsonValue& _prefabValue, PrefabDefinition& _definition) {
		const JsonValue& _animations = _prefabValue["animations"];
		if (!_animations.IsArray()) {
			return;
		}
		for (const auto& _animValue : _animations.GetArray()) {
			if (!_animValue.IsObject()) {
				continue;
			}
			AnimationDefinition _animDef;

			const JsonValue& _name = _animValue["name"];
			if (_name.IsString()) {
				_animDef.Name = _name.GetString();
			}

			const JsonValue& _index = _animValue["index"];
			if (_index.IsNumber()) {
				_animDef.Index = static_cast<int>(_index.GetInt());
			}

			const JsonValue& _frames = _animValue["frames"];
			if (_frames.IsNumber()) {
				_animDef.Frames = static_cast<int>(_frames.GetInt());
			}

			const JsonValue& _loop = _animValue["loop"];
			if (_loop.IsBool()) {
				_animDef.Loop = _loop.GetBool();
			}

			const JsonValue& _priority = _animValue["priority"];
			if (_priority.IsBool()) {
				_animDef.Priority = _priority.GetBool();
			}

			const JsonValue& _frameSize = _animValue["frameSize"];
			if (!_frameSize.IsNull()) {
				TryParseVec2(_frameSize, _animDef.FrameSize);
			}

			if (!_animDef.Name.empty()) {
				_definition.Animations.push_back(std::move(_animDef));
			}
		}
	}

	void ParseComponents(const JsonValue& _prefabValue, PrefabDefinition& _definition) {
		const JsonValue& _components = _prefabValue["components"];
		if (!_components.IsArray()) {
			return;
		}

		for (const auto& _compValue : _components.GetArray()) {
			if (!_compValue.IsObject()) {
				continue;
			}
			ComponentDefinition _component;

			const JsonValue& _type = _compValue["type"];
			if (_type.IsString()) {
				_component.Type = _type.GetString();
			}

			const JsonValue& _params = _compValue["params"];
			if (_params.IsObject()) {
				_component.Params = _params;
			}

			if (!_component.Type.empty()) {
				_definition.Components.push_back(std::move(_component));
			}
		}
	}
}

bool PrefabLoader::LoadFromFile(const std::string& _path, PrefabLibrary& _library)
{
	JsonParseResult _result = JsonParser::ParseFile(_path);
	if (!_result) {
		SDL_Log("PrefabLoader: Failed to parse %s: %s", _path.c_str(), _result.Error.c_str());
		return false;
	}

	const JsonValue& _root = _result.Root;
	if (!_root.IsObject()) {
		SDL_Log("PrefabLoader: Root must be an object in %s", _path.c_str());
		return false;
	}

	const JsonValue& _prefabs = _root["prefabs"];
	if (!_prefabs.IsArray()) {
		SDL_Log("PrefabLoader: Missing 'prefabs' array in %s", _path.c_str());
		return false;
	}

	_library.Clear();

	for (const auto& _prefabValue : _prefabs.GetArray()) {
		if (!_prefabValue.IsObject()) {
			continue;
		}

		PrefabDefinition _definition;

		const JsonValue& _id = _prefabValue["id"];
		if (_id.IsString()) {
			_definition.Id = _id.GetString();
		}

		const JsonValue& _texture = _prefabValue["texture"];
		if (_texture.IsString()) {
			_definition.Texture = _texture.GetString();
		}

		const JsonValue& _width = _prefabValue["width"];
		if (_width.IsNumber()) {
			_definition.Width = static_cast<float>(_width.GetDouble());
		}

		const JsonValue& _height = _prefabValue["height"];
		if (_height.IsNumber()) {
			_definition.Height = static_cast<float>(_height.GetDouble());
		}

		const JsonValue& _position = _prefabValue["position"];
		if (!_position.IsNull()) {
			TryParseVec2(_position, _definition.Position);
		}

		const JsonValue& _tag = _prefabValue["tag"];
		if (_tag.IsString()) {
			_definition.Tag = ParseTag(_tag.GetString());
		}

		ParseAnimations(_prefabValue, _definition);
		ParseComponents(_prefabValue, _definition);

		if (!_definition.Id.empty()) {
			_library.Add(std::move(_definition));
		}
	}

	return true;
}
