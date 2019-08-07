#pragma once

#include "define_types.h"


#include "i_update_draw.h"
#include "event/event.h"
#include "graphics/texture.h"
#include "scene/game_map.h"
#include "actor/player.h"
#include "scene/base_scene.h"

class SplashScene  : public BaseScene
{
public:

	SplashScene(int id,String name);
	virtual ~SplashScene() override;
	void OnLoad() override;
	void OnUnLoad() override;
	void Update() override;
	void Draw() override;
	void Reset();
private:
	int m_State;
};


