#pragma once

#include "define_types.h"


#include "../i_update_draw.h"
#include "../Event/Event.h"
#include "core/Texture.h"
#include "game_map.h"
#include "../actor/Player.h"
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


