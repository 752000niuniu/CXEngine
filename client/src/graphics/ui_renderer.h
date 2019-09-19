#pragma once
#include "singleton.h"
#include <lua.hpp>

class UIRenderer : public Singleton<UIRenderer>
{
public:
	UIRenderer();
	~UIRenderer();

	void Update();
	void Draw();

};

void luaopen_ui_renderer(lua_State* L);