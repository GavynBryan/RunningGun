#include <core/PrefabLibrary.h>

PrefabLibrary::PrefabLibrary()
{
	Reset();
}

void PrefabLibrary::Reset()
{
	PrefabDocument.SetObject();
	Definitions.clear();
}

rapidjson::Document& PrefabLibrary::Document()
{
	return PrefabDocument;
}

const PrefabDefinition* PrefabLibrary::Find(const std::string& _id) const
{
	auto _iter = Definitions.find(_id);
	if (_iter == Definitions.end()) {
		return nullptr;
	}
	return &_iter->second;
}

void PrefabLibrary::Add(PrefabDefinition&& _definition)
{
	if (_definition.Id.empty()) {
		return;
	}
	Definitions.emplace(_definition.Id, std::move(_definition));
}

std::vector<std::string> PrefabLibrary::GetTexturePaths() const
{
	std::vector<std::string> _paths;
	_paths.reserve(Definitions.size());
	for (const auto& _pair : Definitions) {
		_paths.push_back(_pair.second.Texture);
	}
	return _paths;
}
