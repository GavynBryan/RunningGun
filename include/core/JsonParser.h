#pragma once

#include <core/JsonValue.h>
#include <string>

struct JsonParseResult
{
	bool Success = false;
	std::string Error;
	JsonValue Root;

	explicit operator bool() const { return Success; }
};

class JsonParser
{
public:
	static JsonParseResult ParseFile(const std::string& _path);
	static JsonParseResult Parse(const std::string& _json);
};
