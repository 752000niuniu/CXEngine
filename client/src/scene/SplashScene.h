#pragma once

#include "define_types.h"


#include "../IUpdateDraw.h"
#include "../Event/Event.h"
#include "core/Texture.h"
#include "GameMap.h"
#include "../actor/Player.h"
#include "BaseScene.h"

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


