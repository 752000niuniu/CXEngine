#pragma once
#include "singleton.h"

class UIRenderer : public Singleton<UIRenderer>
{
public:
	UIRenderer();
	~UIRenderer();

	void Update();
	void Draw();
};