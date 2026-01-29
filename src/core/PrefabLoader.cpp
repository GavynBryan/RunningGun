#include <core/PrefabLoader.h>
#include <core/JsonUtils.h>
#include <core/PrefabLibrary.h>
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>

namespace {
	bool TryParseVec2(const rapidjson::Value& _value, Vec2& _out) {
		if (!_value.IsArray() || _value.Size() != 2) {
			return false;
		}
		if (!_value[0].IsNumber() || !_value[1].IsNumber()) {
			return false;
		}
		_out = Vec2(static_cast<float>(_value[0].GetDouble()), static_cast<float>(_value[1].GetDouble()));
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

	void ParseAnimations(const rapidjson::Value& _prefabValue, PrefabDefinition& _definition) {
		if (!_prefabValue.HasMember("animations") || !_prefabValue["animations"].IsArray()) {
			return;
		}
		for (const auto& _animValue : _prefabValue["animations"].GetArray()) {
			if (!_animValue.IsObject()) {
				continue;
			}
			AnimationDefinition _animDef;
			if (_animValue.HasMember("name") && _animValue["name"].IsString()) {
				_animDef.Name = _animValue["name"].GetString();
			}
			if (_animValue.HasMember("index") && _animValue["index"].IsInt()) {
				_animDef.Index = _animValue["index"].GetInt();
			}
			if (_animValue.HasMember("frames") && _animValue["frames"].IsInt()) {
				_animDef.Frames = _animValue["frames"].GetInt();
			}
			if (_animValue.HasMember("loop") && _animValue["loop"].IsBool()) {
				_animDef.Loop = _animValue["loop"].GetBool();
			}
			if (_animValue.HasMember("priority") && _animValue["priority"].IsBool()) {
				_animDef.Priority = _animValue["priority"].GetBool();
			}
			if (_animValue.HasMember("frameSize")) {
				TryParseVec2(_animValue["frameSize"], _animDef.FrameSize);
			}
			if (!_animDef.Name.empty()) {
				_definition.Animations.push_back(_animDef);
			}
		}
	}

	void ParseComponents(const rapidjson::Value& _prefabValue, rapidjson::Document::AllocatorType& _allocator, PrefabDefinition& _definition) {
		if (!_prefabValue.HasMember("components") || !_prefabValue["components"].IsArray()) {
			return;
		}

		for (const auto& _compValue : _prefabValue["components"].GetArray()) {
			if (!_compValue.IsObject()) {
				continue;
			}
			ComponentDefinition _component;
			if (_compValue.HasMember("type") && _compValue["type"].IsString()) {
				_component.Type = _compValue["type"].GetString();
			}
			_component.Params.SetObject();
			if (_compValue.HasMember("params") && _compValue["params"].IsObject()) {
				_component.Params.CopyFrom(_compValue["params"], _allocator);
			}
			if (!_component.Type.empty()) {
				_definition.Components.push_back(std::move(_component));
			}
		}
	}
}

bool PrefabLoader::LoadFromFile(const std::string& _path, PrefabLibrary& _library)
{
	std::string _contents;
	if (!JsonUtils::ReadFileContents(_path, _contents)) {
		return false;
	}

	_library.Reset();
	rapidjson::Document& _doc = _library.Document();
	_doc.Parse(_contents.c_str());
	if (_doc.HasParseError()) {
		return false;
	}
	if (!_doc.IsObject() || !_doc.HasMember("prefabs") || !_doc["prefabs"].IsArray()) {
		return false;
	}

	const auto& _prefabs = _doc["prefabs"];
	for (const auto& _prefabValue : _prefabs.GetArray()) {
		if (!_prefabValue.IsObject()) {
			continue;
		}
		PrefabDefinition _definition;
		if (_prefabValue.HasMember("id") && _prefabValue["id"].IsString()) {
			_definition.Id = _prefabValue["id"].GetString();
		}
		if (_prefabValue.HasMember("texture") && _prefabValue["texture"].IsString()) {
			_definition.Texture = _prefabValue["texture"].GetString();
		}
		if (_prefabValue.HasMember("width") && _prefabValue["width"].IsNumber()) {
			_definition.Width = static_cast<float>(_prefabValue["width"].GetDouble());
		}
		if (_prefabValue.HasMember("height") && _prefabValue["height"].IsNumber()) {
			_definition.Height = static_cast<float>(_prefabValue["height"].GetDouble());
		}
		if (_prefabValue.HasMember("position")) {
			TryParseVec2(_prefabValue["position"], _definition.Position);
		}
		if (_prefabValue.HasMember("tag") && _prefabValue["tag"].IsString()) {
			_definition.Tag = ParseTag(_prefabValue["tag"].GetString());
		}

		ParseAnimations(_prefabValue, _definition);
		ParseComponents(_prefabValue, _doc.GetAllocator(), _definition);

		if (!_definition.Id.empty()) {
			_library.Add(std::move(_definition));
		}
	}

	return true;
}
