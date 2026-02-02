#pragma once

#include <simdjson.h>
#include <string>

namespace Json {
	inline simdjson::dom::parser& GetParser() {
		static thread_local simdjson::dom::parser parser;
		return parser;
	}

	inline simdjson::simdjson_result<simdjson::dom::element> Parse(const std::string& _json) {
		return GetParser().parse(simdjson::padded_string(_json));
	}

	inline simdjson::simdjson_result<simdjson::dom::element> ParseFile(const std::string& _path) {
		return GetParser().load(_path);
	}

	inline double GetDouble(simdjson::dom::element _elem, std::string_view _key, double _default = 0.0) {
		auto _result = _elem[_key].get_double();
		return _result.error() ? _default : _result.value();
	}

	inline int64_t GetInt(simdjson::dom::element _elem, std::string_view _key, int64_t _default = 0) {
		auto _result = _elem[_key].get_int64();
		return _result.error() ? _default : _result.value();
	}

	inline std::string_view GetString(simdjson::dom::element _elem, std::string_view _key, std::string_view _default = "") {
		auto _result = _elem[_key].get_string();
		return _result.error() ? _default : _result.value();
	}

	inline bool GetBool(simdjson::dom::element _elem, std::string_view _key, bool _default = false) {
		auto _result = _elem[_key].get_bool();
		return _result.error() ? _default : _result.value();
	}
}
