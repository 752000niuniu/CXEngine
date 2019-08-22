#pragma once


#include "pos.h"
#include "entity.h"
#include "actor/actor.h"
#include "state.h"
#include "ui.h"

class TextView;
class GameMap;
class FrameAnimation;

class Player : public Actor 
{
public:
	Player(uint64_t pid):Actor(pid) {};
	virtual ~Player() {};
};
