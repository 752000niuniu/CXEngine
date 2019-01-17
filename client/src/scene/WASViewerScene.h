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

class WASViewerScene  : public BaseScene
{
public:
	enum State
	{
		Splash, 
		Game
	};

	WASViewerScene(int id,String name);
	void Reset(String wdf);
	~WASViewerScene();
	void OnLoad() override {};
	void OnUnLoad() override {};
	void Update() override;
	void Draw() override;
private:
	int m_State;
	
};


