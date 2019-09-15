#include "actor.h"
#include "scene/game_map.h"
#include "actor/player.h"
#include "scene/scene_manager.h"
#include "utils.h"
#include "lua.hpp"
#include "window.h"
#include "cxmath.h"
#include "actor_manager.h"
#ifndef SIMPLE_SERVER
#include "action.h"
#include "input_manager.h"
#include "animation/sprite.h"
#include "text_renderer.h"
#include "resource_manager.h"
#endif
#include "move.h"
#include "lua.h"
#include "cxlua.h"
#include "script_system.h"
#include "actor_enum.h"


#define ACTOR_METATABLE_NAME "mt_actor"

Actor::Actor(uint64_t pid)
	:BaseGameEntity(pid)
{
	lua_State* L = script_system_get_luastate();
	lua_getglobal(L, "on_actor_reg_props");
	lua_push_actor(L, this);
	int res = lua_pcall(L, 1, 0, 0);
	check_lua_error(L, res);

	m_PatMatrix.clear();

	m_MoveHandle = new MoveHandle(this);
#ifndef SIMPLE_SERVER
	m_SayWidget = new TextView();
	m_SayWidget->PaddingHorizontal = 4;
	m_SayWidget->PaddingVertical = 2;
	m_SayWidget->ShowEmotion = true;
	m_SayWidget->BackgroundResID = RESOURCE_MANAGER_INSTANCE->EncodeWAS(WZIFEWDF, 0xEF073E43 /* 0xA4120EA9*/);

	m_SayDuration = 0;
	m_ASM = new ActionStateMachine(this);  
	INPUT_MANAGER_INSTANCE->RegisterView(this);
	PathMoveAction* action = new PathMoveAction(this);
	m_ASM->ChangeAction(action);
#endif
	

}

Actor::~Actor()
{
	m_PatMatrix.clear();
	SafeDelete(m_MoveHandle);
#ifndef SIMPLE_SERVER
	INPUT_MANAGER_INSTANCE->UnRegisterView(this);
	SafeDelete(m_ASM);
	SafeDelete(m_SayWidget);
#endif
}

void Actor::OnUpdate()
{
	m_MoveHandle->Update();
#ifndef SIMPLE_SERVER
	m_ASM->Update();
	if (m_SayDuration > 0)
	{
		if (m_SayWidget)
		{
			m_SayWidget->OnUpdate();
		}
	}
#endif
}

void Actor::OnDraw()
{
#ifndef SIMPLE_SERVER
	m_ASM->Draw();
	ActorProp& name = GetProperty(PROP_NAME);
	if (!name.toString().empty())
	{
		auto* avatar = m_ASM->GetAvatar();
		if (avatar) {
			auto green = glm::vec3(115 / 255.0f, 1.0f, 137 / 255.0f);
			TextRenderer::GetInstance()->DrawTextC(name.toString().c_str(),
				((int)avatar->Pos.x),
				((int)avatar->Pos.y + 20),
				TextRenderer::CENTER
			);
		}
	}

	if (m_SayDuration > 0)
	{
		int past = (int)WINDOW_INSTANCE->GetDeltaTimeMilliseconds();
		m_SayDuration -= past;

		if (m_SayWidget->Background != nullptr)
		{
			int bgWidth = m_SayWidget->Width;
			int bgHeight = m_SayWidget->Height;
			auto* avatar = m_ASM->GetAvatar();
			m_SayWidget->X = (int)avatar->Pos.x - bgWidth / 2;
			m_SayWidget->Y = (int)avatar->Pos.y - avatar->KeyY - bgHeight;
		}
		m_SayWidget->OnDraw();
	}
#endif
}

void Actor::SetDir(int dir)
{
	m_Props[PROP_DIR] = dir;
#ifndef SIMPLE_SERVER
	auto* avatar = m_ASM->GetAvatar();
	if(avatar){
		if (avatar->GroupCount == 4) {
			m_Props[PROP_DIR] = GMath::Dir8toDir4(dir);
		}
	}
#endif
}

int Actor::GetDir()
{
	int dir = m_Props[PROP_DIR].toInt();
#ifndef SIMPLE_SERVER
	auto* avatar = m_ASM->GetAvatar();
	if (avatar) {
		if (avatar->GroupCount == 4) {
			dir = GMath::Dir8toDir4(dir);
		}
	}
#endif
	return dir;
}

void Actor::ReverseDir()
{
	int dir = m_Props[PROP_DIR].toInt();
	SetDir(GMath::GetReverseDir(dir));
}

void Actor::SetActionID(int action)
{
	if (m_Props[PROP_ACTOR_TYPE].toInt() == ACTOR_TYPE_PLAYER) {
		m_Props[PROP_ACTION_ID] = action;
	}
	else {
		if (action_is_emotion_action(action))return;
		m_Props[PROP_ACTION_ID] = action;
	}
}

void Actor::SetPos(float x, float y)
{
	if (IsCombat()) {
		m_Props[PROP_COMBAT_POS].v2[0] = x;
		m_Props[PROP_COMBAT_POS].v2[1] = y;
	}
	else {
		m_Props[PROP_POS].v2[0] = x;
		m_Props[PROP_POS].v2[1] = y;
	}
}

void Actor::SetPos(Pos p)
{
	SetPos(p.x, p.y);
}

Pos Actor::GetPos()
{
	ActorProp& prop = m_Props[IsCombat() ? PROP_COMBAT_POS : PROP_POS];
	return  Pos(prop.v2[0], prop.v2[1]);
}

void Actor::SetMoveToPos(Pos dest)
{
	m_Props[PROP_MOVE_TO_POS].v2[0] = dest.x;
	m_Props[PROP_MOVE_TO_POS].v2[1] = dest.y;
}

Pos Actor::GetMoveToPos()
{
	return { m_Props[PROP_MOVE_TO_POS].v2[0] ,m_Props[PROP_MOVE_TO_POS].v2[1] };
}

float Actor::GetWidth()
{
#ifndef SIMPLE_SERVER
	auto* avatar = m_ASM->GetAvatar();
	if (!avatar)return 0;
	return (float)avatar->Width;
#else
	return 0;
#endif
}

float Actor::GetHeight()
{
#ifndef SIMPLE_SERVER
	auto* avatar = m_ASM->GetAvatar();
	if (!avatar)return 0;
	return (float)avatar->Height;
#else
	return 0;
#endif
}

int Actor::GetDirByDegree(float degree)
{
#ifndef SIMPLE_SERVER
	int dircnt = m_ASM->GetDirCount();
	if (dircnt == 8) {
		return GMath::Astar_GetDir(degree);
	}
	else {
		return GMath::Astar_GetDir4(degree);
	}
#else
	return GMath::Astar_GetDir(degree);
#endif
}

void Actor::SetLocal(bool local)
{
	actor_manager_set_local_player(this);
}

bool Actor::IsLocal()
{
	return actor_manager_is_local_player(this);
}


float Actor::GetMoveDestDistSquare(Pos dest)
{
	Pos& pos = GetPos();
	return ::GMath::Astar_GetDistanceSquare(pos.x,pos.y, dest.x, dest.y);
}

float Actor::GetMoveDestAngle(Pos dest)
{
	Pos& pos = GetPos();
	return ::GMath::Astar_GetAngle(pos.x,pos.y, dest.x, dest.y);
}

BaseScene* Actor::GetScene()
{
	return scene_manager_fetch_scene(m_Props[PROP_SCENE_ID].toInt());
}

CXString Actor::GetWeaponAvatarID()
{
	if (m_Props[PROP_ACTOR_TYPE].toInt() == ACTOR_TYPE_NPC)
		return "";
	return m_Props[PROP_WEAPON_AVATAR_ID].toString();
}
#ifndef SIMPLE_SERVER
Bound Actor::GetViewBounds()
{
	auto* avatar = m_ASM->GetAvatar();
	if (avatar == nullptr) return { 0,0,0,0 };
	Pos pos = avatar->Pos;
	pos.x -= avatar->KeyX;
	pos.y -= avatar->KeyY;
	return Bound{ pos.x, (pos.x + avatar->Width),
		pos.y,(pos.y + avatar->Height) };
}

bool Actor::CheckDrag(int dx, int dy)
{
	return pow(dx, 2) + pow(dy, 2) >= 16;
}

void Actor::OnDragMove(int dx, int dy)
{
	TranslateX((float)dx);
	TranslateY((float)dy);
}

void Actor::Say(std::string Text)
{
	std::wstring wText = utils::StringToWstring(Text);
	m_SayDuration = 1000 * 10;// *24;
	m_SayWidget->Text = wText;
	m_SayWidget->TextCache = std::vector<uint32_t>(wText.begin(), wText.end());
}
#endif

Actor* lua_check_actor(lua_State*L, int index)
{
	Actor** ptr = (Actor**)lua_touserdata(L, index);
	return *ptr;
}

int actor_destroy(lua_State * L)
{
	Actor** ptr = (Actor**)lua_touserdata(L, 1);
	Actor* actor = *ptr;
	if (actor) {
		SafeDelete(actor);
	}
	return 0;
}
int actor_update(lua_State * L) {
	Actor* actor = lua_check_actor(L, 1);
	actor->OnUpdate();
	return 0;
}

int actor_draw(lua_State * L) {
	Actor* actor = lua_check_actor(L, 1);
	actor->OnDraw();
	return 0;
}

int actor_set_dir(lua_State* L)
{
	Actor* actor = lua_check_actor(L, 1);
	lua_Number dir = lua_tonumber(L, 2);
	actor->SetDir((int)dir);
	return 0;
}
int actor_get_dir(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
	lua_pushnumber(L, actor->GetDir());
	return 1;
}

int actor_reverse_dir(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
	actor->ReverseDir();
	return 0;
}

int actor_set_local(lua_State* L)
{
	Actor* actor = lua_check_actor(L, 1);
	bool  local = (bool)lua_toboolean(L, 2);
	actor->SetLocal(local);
	return 0;
}

int actor_is_local(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
	lua_pushboolean(L, actor_manager_is_local_player(actor));
	return 1;
}


int actor_get_id(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
	lua_pushinteger(L, actor->GetID());
	return 1;
}

int actor_set_action_id(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
	int actionID = (int)lua_tointeger(L, 2);
	actor->SetActionID(actionID);
#ifndef SIMPLE_SERVER
	Action* action = new Action(actor);
	actor->GetASM()->ChangeAction(action);
#endif
	return 0;
}

int actor_get_action_id(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
	lua_pushinteger(L, actor->GetProperty(PROP_ACTION_ID).i);
	return 1;
}

int actor_translate_x(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
	float x = (float)luaL_optnumber(L, 2, 0.f);
	actor->TranslateX(x);
	return 0;
}

int actor_translate_y(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
	float y = (float)luaL_optnumber(L, 2, 0.f);
	actor->TranslateY(y);
	return 0;
}


int actor_move_to(lua_State* L){
	Actor* actor = lua_check_actor(L, 1);
	float x = (float)lua_tonumber(L, 2);
	float y = (float)lua_tonumber(L, 3);
#ifndef SIMPLE_SERVER
	PathMoveAction* action = dynamic_cast<PathMoveAction*>(actor->GetASM()->GetAction());
	if (action == nullptr){
		action = new PathMoveAction(actor);
		actor->GetASM()->ChangeAction(action);
	}
	actor->GetASM()->Update();
#endif
	actor->GetMoveHandle()->MoveTo(x, y);
	return 0;
}
int actor_say(lua_State* L) {
#ifndef SIMPLE_SERVER
	Actor* actor = lua_check_actor(L, 1);
	const char* msg = lua_tostring(L, 2);
	actor->Say(msg);
#endif
	return 0;
}


int actor_get_width(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
	lua_pushnumber(L, actor->GetWidth());
	return 1;
}

int actor_get_height(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
	lua_pushnumber(L, actor->GetHeight());
	return 1;
}

int actor_clear_frames(lua_State* L) {
	return 0;
}

int actor_play_attack(lua_State*L){
#ifndef SIMPLE_SERVER 
	Actor* actor = lua_check_actor(L, 1);
	AttackAction* action = new AttackAction(actor);
	actor->GetASM()->ChangeAction(action);
#endif
	return 0;
}

int actor_play_cast(lua_State*L) {
#ifndef SIMPLE_SERVER 
	Actor* actor = lua_check_actor(L, 1);
	uint64_t skill = (uint64_t)lua_tointeger(L, 3);
	actor->SetProperty(PROP_CAST_ID, skill);
	CastAction* action = new CastAction(actor, skill);
	actor->GetASM()->ChangeAction(action);
#endif
	return 0;
}

int actor_set_time_interval(lua_State*L) {
#ifndef SIMPLE_SERVER
	Actor* actor = lua_check_actor(L, 1);
	float ti = (float)lua_tonumber(L, 2);
	actor->GetASM()->SetTimeInterval(ti);
#endif
	return 0;
}

int actor_get_avatar(lua_State*L) {
#ifndef SIMPLE_SERVER
	Actor* actor = lua_check_actor(L, 1);
	auto* avatar = actor->GetASM()->GetAvatar();
	if (avatar == nullptr)return 0;
	lua_push_animation(L, avatar);
	return 1;
#else
	return 0;
#endif
}

int actor_get_weapon(lua_State*L) {
#ifndef SIMPLE_SERVER
	Actor* actor = lua_check_actor(L, 1);
	auto* weapon = actor->GetASM()->GetWeapon();
	if (weapon == nullptr)return 0;
	lua_push_animation(L, weapon);
	return 1;
#else
	return 0;
#endif
}

int actor_change_pal_matrix(lua_State*L) {
#ifndef SIMPLE_SERVER
	Actor* actor = lua_check_actor(L, 1);
	int len = (int)luaL_len(L, 2);
	std::vector<NE::PalSchemePart> patMatrix;
	for (int i = 1; i<=len; i++) {
		NE::PalSchemePart seg_matrix;
		lua_geti(L, -1, i);
		
		lua_getfield(L, -1, "from");
		seg_matrix.from = (uint16_t)lua_tointeger(L, -1);
		lua_pop(L, 1);

		lua_getfield(L, -1, "to");
		seg_matrix.to = (uint16_t)lua_tointeger(L, -1);
		lua_pop(L, 1);

		lua_getfield(L, -1, "mat");
		assert(luaL_len(L, -1) == 9);
		seg_matrix.mat.resize(9);
		seg_matrix.mat.clear();
		for (int mat_i = 1; mat_i <= 9; mat_i++) {
			lua_geti(L, -1, mat_i);
			uint16_t val = (uint16_t)lua_tointeger(L, -1);
			seg_matrix.mat.push_back(val);
			lua_pop(L, 1);
		}
		lua_pop(L, 1);
		
		lua_pop(L, 1);
		patMatrix.push_back(seg_matrix);
	}
	actor->SetPalette(patMatrix);
	actor->GetASM()->Reset();
	return 0;
#else
	return 0;
#endif
}

int actor_get_pal_matrix(lua_State*L) {
#ifndef SIMPLE_SERVER
	Actor* actor = lua_check_actor(L, 1);
	return 0;
#else
	return 0;
#endif
}

int actor_reset_asm(lua_State*L){
	Actor* actor = lua_check_actor(L, 1);
#ifndef SIMPLE_SERVER
	actor->GetASM()->Reset();
#endif
	return 0;
}

int actor_clear_pal_matrix(lua_State*L) {
#ifndef SIMPLE_SERVER
	Actor* actor = lua_check_actor(L, 1);
	return 0;
#else
	return 0;
#endif
}

int actor_set_target(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
	Actor* target = lua_check_actor(L, 2);
	actor->SetTarget(target);
	return 0;
}

int actor_get_target(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
	lua_push_actor(L, actor->GetTarget());
	return 1;
}

//{ "__gc",actor_destroy },
luaL_Reg mt_actor[] = {
	{ "Destroy",actor_destroy },
{ "Update",actor_update },
{ "Draw",actor_draw },
{ "GetWidth", actor_get_width },
{ "GetHeight", actor_get_height },
{ "GetID", actor_get_id },
{ "SetDir", actor_set_dir },
{ "GetDir", actor_get_dir },
{ "ReverseDir", actor_reverse_dir },
{ "SetLocal", actor_set_local },
{ "IsLocal", actor_is_local },
{ "SetActionID", actor_set_action_id },
{ "GetActionID", actor_get_action_id },
{ "TranslateX", actor_translate_x },
{ "TranslateY", actor_translate_y },
{ "MoveTo", actor_move_to },
{ "Say", actor_say },
{ "ClearFrames", actor_clear_frames },
{ "SetTarget", actor_set_target},
{ "GetTarget", actor_get_target},
{ "PlayAttack", actor_play_attack },
{ "PlayCast", actor_play_cast },
{ "SetTimeInterval", actor_set_time_interval },
{ "GetAvatar", actor_get_avatar },
{ "GetWeapon", actor_get_weapon },
{ "ChangePalMatrix", actor_change_pal_matrix},
{ "GetPalMatrix", actor_get_pal_matrix},
{ "ClearPalMatrix", actor_clear_pal_matrix},
{ "RegProperty", actor_reg_prop},
{ "GetProperty", actor_get_prop},
{ "SetProperty", actor_set_prop},
{ "ResetASM", actor_reset_asm},
{ NULL, NULL }
};

void lua_push_actor(lua_State*L, Actor* actor)
{
	Actor** ptr = (Actor**)lua_newuserdata(L, sizeof(Actor*));
	*ptr = actor;
	if (luaL_newmetatable(L, ACTOR_METATABLE_NAME)) {
		luaL_setfuncs(L, mt_actor, 0);
		lua_pushvalue(L, -1);
		lua_setfield(L, -2, "__index");
	}
	lua_setmetatable(L, -2);
}


int actor_get_metatable(lua_State* L){
	if (luaL_newmetatable(L, ACTOR_METATABLE_NAME)) {
		luaL_setfuncs(L, mt_actor, 0);
		lua_pushvalue(L, -1);
		lua_setfield(L, -2, "__index");
	}
	return 1;
}

int action_system_get_action_size() {
	return (int)action_get_size();
}

std::string action_system_get_action_name(int action) {
	return action_get_name(action);
}

void luaopen_actor(lua_State* L)
{
	script_system_register_luac_function(L, actor_get_metatable);

	script_system_register_function(L, action_system_get_action_size);
	script_system_register_function(L, action_system_get_action_name);


#define REG_ENUM(e) (lua_pushinteger(L, e), lua_setglobal(L, #e))
	REG_ENUM(ACTOR_TYPE_DEFAULT);
	REG_ENUM(ACTOR_TYPE_PLAYER);
	REG_ENUM(ACTOR_TYPE_PET);
	REG_ENUM(ACTOR_TYPE_NPC);

	REG_ENUM(ACTION_IDLE);
	REG_ENUM(ACTION_WALK);
	REG_ENUM(ACTION_SIT);
	REG_ENUM(ACTION_ANGRY);
	REG_ENUM(ACTION_SAYHI);
	REG_ENUM(ACTION_DANCE);
	REG_ENUM(ACTION_SALUTE);
	REG_ENUM(ACTION_CLPS);
	REG_ENUM(ACTION_CRY);
	REG_ENUM(ACTION_BATIDLE);
	REG_ENUM(ACTION_ATTACK);
	REG_ENUM(ACTION_CAST);
	REG_ENUM(ACTION_BEHIT);
	REG_ENUM(ACTION_RUNTO);
	REG_ENUM(ACTION_RUNBACK);
	REG_ENUM(ACTION_DEFEND);


	REG_ENUM(ASM_ACTION);
	REG_ENUM(ASM_ATTACK_ACTION);
	REG_ENUM(ASM_CAST_ACTION);
	REG_ENUM(ASM_BEHIT_ACTION);
	REG_ENUM(ASM_BECAST_ACTION);
	REG_ENUM(ASM_DEAD_FLY_ACTION);
	REG_ENUM(ASM_PATH_MOVE_ACTION);
		
	REG_ENUM(WEAPON_SPEAR);
	REG_ENUM(WEAPON_AXE);
	REG_ENUM(WEAPON_SWORD);
	REG_ENUM(WEAPON_DBSWORDS);
	REG_ENUM(WEAPON_RIBBON);
	REG_ENUM(WEAPON_CLAW);
	REG_ENUM(WEAPON_FAN);
	REG_ENUM(WEAPON_WAND);
	REG_ENUM(WEAPON_HAMMER);
	REG_ENUM(WEAPON_WHIP);
	REG_ENUM(WEAPON_RING);
	REG_ENUM(WEAPON_KNIFE);
	REG_ENUM(WEAPON_STAFF);
	REG_ENUM(WEAPON_JEWEL);
	REG_ENUM(WEAPON_BOW);


	REG_ENUM(ROLE_FYN);
	REG_ENUM(ROLE_YNX);
	REG_ENUM(ROLE_XYS);
	REG_ENUM(ROLE_JXK);
	REG_ENUM(ROLE_HMR);
	REG_ENUM(ROLE_GJL);
	REG_ENUM(ROLE_JMW);
	REG_ENUM(ROLE_HTG);
	REG_ENUM(ROLE_WTJ);
	REG_ENUM(ROLE_XCE);
	REG_ENUM(ROLE_STB);
	REG_ENUM(ROLE_LTZ);

	REG_ENUM(DIR_N);
	REG_ENUM(DIR_S);
	REG_ENUM(DIR_W);
	REG_ENUM(DIR_E);
	REG_ENUM(DIR_NE);
	REG_ENUM(DIR_NW);
	REG_ENUM(DIR_SE);
	REG_ENUM(DIR_SW);
#undef REG_ENUM
	luaopen_actor_enum(L);
}
