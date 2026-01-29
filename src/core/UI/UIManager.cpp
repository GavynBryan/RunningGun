#include <core/UIManager.h>
#include <algorithm>

UIManager::UIManager(float _screenWidth, float _screenHeight)
	: ScreenWidth(_screenWidth)
	, ScreenHeight(_screenHeight)
{
}

UIManager::~UIManager()
{
}

void UIManager::SetScreenSize(float _width, float _height)
{
	ScreenWidth = _width;
	ScreenHeight = _height;
}

void UIManager::RemoveElement(UIElement* _element)
{
	Elements.erase(
		std::remove_if(Elements.begin(), Elements.end(),
			[_element](const std::unique_ptr<UIElement>& _e) {
				return _e.get() == _element;
			}),
		Elements.end()
	);
}

void UIManager::Clear()
{
	Elements.clear();
}

void UIManager::Update(float _deltaTime)
{
	for (auto& _element : Elements) {
		if (_element->IsVisible()) {
			_element->Update(_deltaTime);
		}
	}
}

void UIManager::Render(SDL_Renderer* _renderer)
{
	for (auto& _element : Elements) {
		if (_element->IsVisible()) {
			_element->Render(_renderer, ScreenWidth, ScreenHeight);
		}
	}
}
