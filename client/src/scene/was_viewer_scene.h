#pragma once

#include "define_types.h"


#include "i_update_draw.h"
#include "event/event.h"
#include "graphics/texture.h"
#include "game_map.h"
#include "actor/player.h"
#include "base_scene.h"

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


