#pragma once


#include "pos.h"
#include "entity.h"
#include "actor/action.h"
#include "actor/actor.h"
#include "scene/game_map.h"
#include "state.h"

class Player : public Actor
{
public:

	Player(uint64_t id = 0);
	virtual ~Player();



};

