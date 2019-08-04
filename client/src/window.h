#pragma once

#include "game.h"
#include "singleton.h"

class Window : public Singleton<Window>
{
public:
	Window();
	~Window();
	
	void Init(int w,int h);
	void Show();

	GLFWwindow* GetGLFWwindow(){ return m_pWindow;};
	int GetWidth() { return m_Width; };
	int GetHeight() { return m_Height; };
	int GetWindowWidth() { return m_WindowWidth; };
	int GetWindowHeight() { return m_WindowHeight; };
	float GetCenterX() { return GetWidth()/ 2.f; }
	float GetCenterY() { return GetHeight()/ 2.f; }
	float GetDeltaTime();
	float GetDeltaTimeMilliseconds();
	void OnFrameBufferSizeCallback(int width, int height);
private:
	int m_Width;
	int m_Height;
	int m_WindowWidth;
	int m_WindowHeight;
	float m_FPS;
	GLFWwindow *m_pWindow;
};


#define WINDOW_INSTANCE Window::GetInstance()

void luaopen_window(lua_State* L);

