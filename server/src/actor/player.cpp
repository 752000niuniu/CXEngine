#include "player.h"
#include "logger.h"
#include "message.h"
#include "combat/Combat.h"
#include "combat/Skill.h"
#include "cxrandom.h"
#include "profile.h"
#include "file_loading.h"
#include "scene/scene_manager.h"
#include "scene/scene.h"
#include "cxmath.h"
#include "utils.h"
#include "window.h"


Player::Player(uint64_t id) :
	Actor(id)
{

}


Player::~Player()
{

}


