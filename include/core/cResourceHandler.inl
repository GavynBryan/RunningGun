#include <SFML/Graphics.hpp>
template<typename Resource>
void ResourceHandler<Resource>::load(const std::string& _filename) 
{
	std::unique_ptr<Resource> res(new Resource());
	if (!res->loadFromFile(_filename))
		throw std::runtime_error("Could not load " + _filename);

	auto inserted = mResources.insert(std::make_pair(_filename, std::move(res)));
	assert(inserted.second);
}

template<typename Resource>
Resource& ResourceHandler<Resource>::get(const std::string& _filename)
{
	auto found = mResources.find(_filename);
	assert(found != mResources.end());

	return *found->second;
}

template<typename Resource>
void ResourceHandler<Resource>::flush() 
{
	mResources.clear();
}

