#pragma once
#include <SDL3/SDL.h>
#include <vector>
#include <memory>
#include <string>
#include <core/UI/UIElement.h>

class UIManager
{
private:
	std::vector<std::unique_ptr<UIElement>>		Elements;
	float										ScreenWidth;
	float										ScreenHeight;

public:
	UIManager(float _screenWidth, float _screenHeight);
	~UIManager();

	void			SetScreenSize(float _width, float _height);
	float			GetScreenWidth() const { return ScreenWidth; }
	float			GetScreenHeight() const { return ScreenHeight; }

	template<typename T, typename... Args>
	T*				AddElement(Args&&... _args);

	void			RemoveElement(UIElement* _element);
	void			Clear();

	void			Update(float _deltaTime);
	void			Render(SDL_Renderer* _renderer);
};

template<typename T, typename... Args>
T* UIManager::AddElement(Args&&... _args)
{
	static_assert(std::is_base_of<UIElement, T>::value, "T must derive from UIElement");
	auto _element = std::make_unique<T>(std::forward<Args>(_args)...);
	T* _ptr = _element.get();
	Elements.push_back(std::move(_element));
	return _ptr;
}
