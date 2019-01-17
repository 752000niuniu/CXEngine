#pragma once
#include "defines.h"
#include "defineTypes.h"
#include "defines.h"

#include "../IUpdateDraw.h"
#include "../Event/Event.h"
#include "core/Texture.h"
#include "GameMap.h"
#include "../actor/Player.h"
#include "BaseScene.h"

class UIScene : public BaseScene
{
public:
	enum State
	{
		Splash,
		Game
	};

	UIScene(int id, String name);
	~UIScene();
	void OnLoad() override;
	void OnUnLoad() override;
	void Update() override;
	void Draw() override;
private:
//	int m_State;
//	bool m_bDragStart;
};


