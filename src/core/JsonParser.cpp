#include <core/JsonParser.h>
#include <simdjson.h>
#include <fstream>
#include <sstream>

namespace {
	JsonValue ConvertElement(simdjson::dom::element _element);

	JsonValue ConvertArray(simdjson::dom::array _array)
	{
		JsonValue::Array _result;
		_result.reserve(_array.size());
		for (auto _elem : _array) {
			_result.push_back(ConvertElement(_elem));
		}
		return JsonValue(std::move(_result));
	}

	JsonValue ConvertObject(simdjson::dom::object _object)
	{
		JsonValue::Object _result;
		for (auto _field : _object) {
			_result.emplace(std::string(_field.key), ConvertElement(_field.value));
		}
		return JsonValue(std::move(_result));
	}

	JsonValue ConvertElement(simdjson::dom::element _element)
	{
		switch (_element.type()) {
		case simdjson::dom::element_type::NULL_VALUE:
			return JsonValue();
		case simdjson::dom::element_type::BOOL:
			return JsonValue(_element.get_bool().value());
		case simdjson::dom::element_type::INT64:
			return JsonValue(_element.get_int64().value());
		case simdjson::dom::element_type::UINT64:
			return JsonValue(static_cast<int64_t>(_element.get_uint64().value()));
		case simdjson::dom::element_type::DOUBLE:
			return JsonValue(_element.get_double().value());
		case simdjson::dom::element_type::STRING:
			return JsonValue(std::string(_element.get_string().value()));
		case simdjson::dom::element_type::ARRAY:
			return ConvertArray(_element.get_array().value());
		case simdjson::dom::element_type::OBJECT:
			return ConvertObject(_element.get_object().value());
		}
		return JsonValue();
	}
}

JsonParseResult JsonParser::ParseFile(const std::string& _path)
{
	std::ifstream _file(_path);
	if (!_file) {
		return { false, "Failed to open file: " + _path, JsonValue() };
	}
	std::ostringstream _stream;
	_stream << _file.rdbuf();
	return Parse(_stream.str());
}

JsonParseResult JsonParser::Parse(const std::string& _json)
{
	simdjson::dom::parser _parser;
	simdjson::padded_string _padded(_json);
	auto _result = _parser.parse(_padded);

	if (_result.error()) {
		return { false, std::string(simdjson::error_message(_result.error())), JsonValue() };
	}

	return { true, "", ConvertElement(_result.value()) };
}
