#include <core/JsonUtils.h>
#include <fstream>
#include <sstream>

namespace JsonUtils
{
	bool ReadFileContents(const std::string& _path, std::string& _output)
	{
		std::ifstream _file(_path);
		if (!_file) {
			return false;
		}
		std::ostringstream _stream;
		_stream << _file.rdbuf();
		_output = _stream.str();
		return true;
	}
}
