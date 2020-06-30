#pragma once
#include <map>
#include <string>
#include <memory>
#include <stdexcept>
#include <cassert>

template<typename Resource>
class ResourceHandler
{
public:
	void					load(const std::string& _filename);
	Resource&				get(const std::string& _filename);

	void					flush();

private:
	std::map<std::string, std::unique_ptr<Resource>> mResources;
};

#include "cResourceHandler.inl"