#pragma once

#include <string>

namespace JsonUtils
{
	bool ReadFileContents(const std::string& _path, std::string& _output);
}
