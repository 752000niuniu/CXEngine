#include "animation/sprite.h"
#include <NESupport.h>
#include "texture_manager.h"
#include "resource_manager.h"
#include "window.h"
#include "sprite_renderer.h"
#include "input_manager.h"
#include "logger.h"

String UtilsGetFramePath(Sprite* m_pSprite, int index)
{
	if (m_pSprite)
	{
		return m_pSprite->mPath + std::string("/" + std::to_string(index));
	}
	return "";
}

Texture* UtilsGetFrameTexture(Sprite* m_pSprite, int index)
{
	if (!m_pSprite)return nullptr;
	if (index >= m_pSprite->mFrameSize * m_pSprite->mGroupSize)return nullptr;
	auto path = UtilsGetFramePath(m_pSprite, index);
	auto& frame = m_pSprite->mFrames[index];
	return TextureManager::GetInstance()->LoadTexture(path, frame.width, frame.height, true, (uint8_t*)frame.src.data());
}

BaseSprite::BaseSprite(uint64_t resoureID)
{
	if (resoureID == 0) {
		m_pSprite = nullptr;
		return;
	}
	m_pSprite = RESOURCE_MANAGER_INSTANCE->LoadWASSpriteByID(resoureID, true);
	Width = m_pSprite->mWidth;
	Height = m_pSprite->mHeight;
	KeyX = m_pSprite->mKeyX;
	KeyY = m_pSprite->mKeyY;
	TotalFrames = m_pSprite->mGroupSize*m_pSprite->mFrameSize;
	GroupFrameCount = m_pSprite->mFrameSize;
	GroupCount = m_pSprite->mGroupSize;

	CurrentFrame = 0;
	for (int i = 0; i < TotalFrames; i++)
	{
		UtilsGetFrameTexture(m_pSprite, i);
	}
	Pos = { 0,0 };
	FrameInterval = 0.064f;
	PlayTime = 0.f;
	Dir = 0;
	bPlay = true;
	bLoop = true;
	bGroupEndUpdate = false;
	bEnableDrag = false;

	int max_dx = 0;
	int max_frame = 0;
	for (int i = 0; i < GroupFrameCount-1;i++) {
		int dx = ( m_pSprite->mFrames[i ].key_x - m_pSprite->mFrames[i+1].key_x);
		if (max_dx < dx) {
			max_dx = dx;
			max_frame = i + 1;
		}
	}
	AttackKeyFrame = max_frame;
}

BaseSprite::BaseSprite(uint32_t pkg, uint32_t wasID) :BaseSprite(RESOURCE_MANAGER_INSTANCE->EncodeWAS(pkg, wasID)) {}

BaseSprite::~BaseSprite()
{
	INPUT_MANAGER_INSTANCE->UnRegisterView(this);
	m_pSprite = nullptr;
}

void BaseSprite::Update()
{
	if (!m_pSprite)return;
	bGroupEndUpdate = false;
	bFrameUpdated = false;
	if (bPlay) {
		float dt = WINDOW_INSTANCE->GetDeltaTime();
		if (m_Pause) {
			m_PauseTime = m_PauseTime - WINDOW_INSTANCE->GetDeltaTimeMilliseconds();
			if (m_PauseTime <= 0) {
				m_Pause = false;
			}
			return;
		}

		PlayTime = PlayTime + dt;
		if (PlayTime >= FrameInterval)
		{
			bFrameUpdated = true;
			PlayTime = (PlayTime - std::floor(PlayTime / FrameInterval)*FrameInterval);
			CurrentFrame = CurrentFrame + 1;
			if (CurrentFrame >= GroupFrameCount) {
				bGroupEndUpdate = true;
				if (bLoop) {
					CurrentFrame = 0;
				}
				else {
					CurrentFrame = CurrentFrame - 1;
					bPlay = false;
				}
			}
		}
	}
}

void BaseSprite::Draw()
{
	if (!m_pSprite)return;
	auto* texture = UtilsGetFrameTexture(m_pSprite, Dir*GroupFrameCount + CurrentFrame);
	if (texture)
	{
		auto& frame = m_pSprite->mFrames[Dir*GroupFrameCount + CurrentFrame];
		int kx = (m_pSprite->mKeyX - frame.key_x);
		int ky = (m_pSprite->mKeyY - frame.key_y);
		SPRITE_RENDERER_INSTANCE->DrawFrameSprite(texture,
			glm::vec2(Pos.x + kx, Pos.y + ky),
			glm::vec2(frame.width, frame.height), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
	}
}


void BaseSprite::SetLoop(bool loop)
{
	bLoop = loop;
}

void BaseSprite::Reset()
{
	PlayTime = 0;
	CurrentFrame = 0;
	//FrameInterval = 0.016f * 4;
}

void BaseSprite::Stop()
{
	bPlay = false;
}

void BaseSprite::Play()
{
	bPlay = true;
}


void BaseSprite::Pause(int ms)
{
	m_PauseTime = ms;
	m_Pause = true;
}

NE::Sprite::Sequence* BaseSprite::GetFrame(int index /*= -1*/)
{
	if (!m_pSprite)return nullptr;
	if (index == -1)index = CurrentFrame;
	if (index >= m_pSprite->mFrames.size())return 0;
	return &m_pSprite->mFrames[index];
}
int BaseSprite::GetFrameKeyX(int index /*= -1*/)
{
	if (!m_pSprite)return 0;
	if (index == -1)index = CurrentFrame;
	int frame = Dir * GroupFrameCount + index;
	if (frame >= m_pSprite->mFrames.size())return 0;
	return m_pSprite->mFrames[frame].key_x;
}

int BaseSprite::GetFrameKeyY(int index /*= -1*/)
{
	if (!m_pSprite)return 0;
	if (index == -1)index = CurrentFrame;
	int frame = Dir * GroupFrameCount + index;
	if (frame >= m_pSprite->mFrames.size())return 0;
	return m_pSprite->mFrames[frame].key_y;
}


int BaseSprite::GetFrameWidth(int index /*= -1*/)
{
	if (!m_pSprite)return 0;
	if (index == -1)index = CurrentFrame;
	int frame = Dir * GroupFrameCount + index;
	if (frame >= m_pSprite->mFrames.size())return 0;
	return m_pSprite->mFrames[frame].width;
}


int BaseSprite::GetFrameHeight(int index /*= -1*/)
{
	if (!m_pSprite)return 0;
	if (index == -1)index = CurrentFrame;
	int frame = Dir * GroupFrameCount + index;
	if (frame >= m_pSprite->mFrames.size())return 0;
	return m_pSprite->mFrames[frame].height;
}

void BaseSprite::EnableDrag(bool enable)
{
	bEnableDrag = enable;
	if(bEnableDrag){
		INPUT_MANAGER_INSTANCE->RegisterView(this);
	}else{
		INPUT_MANAGER_INSTANCE->UnRegisterView(this);
	}
}
Bound BaseSprite::GetViewBounds()
{
	return Bound{ Pos.x, (Pos.x + Width),
		Pos.y,(Pos.y + Height) };
}

bool BaseSprite::CheckDrag(int dx, int dy)
{
	return bEnableDrag && pow(dx, 2) + pow(dy, 2) >= 16;
}

void BaseSprite::OnDragMove(int dx, int dy)
{
	Pos.x += (float)dx;
	Pos.y += (float)dy;
}



Animation::Animation(uint64_t resoureID /*= 0*/) :BaseSprite(resoureID)
{
	Visible = true;
}

void Animation::Update()
{
	if (Visible) {
		float dt = WINDOW_INSTANCE->GetDeltaTime();
		PlayTime = PlayTime + dt;
		if (PlayTime >= FrameInterval)
		{
			PlayTime = (PlayTime - std::floor(PlayTime / FrameInterval)*FrameInterval);
			CurrentFrame = CurrentFrame + 1;
			if (CurrentFrame >= GroupFrameCount) {
				Visible = false;
			}
		}
	}
}

void Animation::Draw()
{
	if (Visible)
	{
		BaseSprite::Draw();
	}
}

BaseSprite* lua_check_base_sprite(lua_State*L, int index)
{
	BaseSprite** ptr = (BaseSprite**)lua_touserdata(L, index);
	return *ptr;
}

int base_sprite_update(lua_State* L) {
	auto* base_sprite = lua_check_base_sprite(L, 1);
	base_sprite->Update();
	return 0;
}

int base_sprite_draw(lua_State* L) {
	auto* base_sprite = lua_check_base_sprite(L, 1);
	base_sprite->Draw();
	return 0;
}

int base_sprite_set_pos(lua_State* L) {
	auto* base_sprite = lua_check_base_sprite(L, 1);
	auto x = (float)lua_tonumber(L, 2);
	auto y = (float)lua_tonumber(L, 3);
	base_sprite->Pos.x = x;
	base_sprite->Pos.y = y;
	return 0;
}

int base_sprite_get_pos(lua_State* L) {
	auto* base_sprite = lua_check_base_sprite(L, 1);
	lua_pushnumber(L, base_sprite->Pos.x);
	lua_pushnumber(L, base_sprite->Pos.y);
	return 2;
}

int base_sprite_set_dir(lua_State* L) {
	auto* base_sprite = lua_check_base_sprite(L, 1);
	auto dir = (int)lua_tointeger(L, 2);
	base_sprite->Dir = dir;
	return 0;
}

int base_sprite_get_dir(lua_State* L) {
	auto* base_sprite = lua_check_base_sprite(L, 1);
	lua_pushinteger(L, base_sprite->Dir);
	return 1;
}

int base_sprite_stop(lua_State* L) {
	auto* base_sprite = lua_check_base_sprite(L, 1);
	base_sprite->Stop();
	return 0;
}

int base_sprite_start(lua_State* L) {
	auto* base_sprite = lua_check_base_sprite(L, 1);
	base_sprite->Play();
	return 0;
}


int base_sprite_set_frame_interval(lua_State* L) {
	auto* base_sprite = lua_check_base_sprite(L, 1);
	auto interval = (float)lua_tonumber(L, 2);
	base_sprite->FrameInterval = interval;
	base_sprite->CurrentFrame = 0;
	base_sprite->PlayTime = 0.f;
	return 0;
}

int base_sprite_get_frame_interval(lua_State* L) {
	auto* base_sprite = lua_check_base_sprite(L, 1);
	lua_pushnumber(L, base_sprite->FrameInterval);
	return 1;
}

int base_sprite_get_width(lua_State* L) {
	auto* base_sprite = lua_check_base_sprite(L, 1);
	lua_pushinteger(L, base_sprite->Width);
	return 1;
}

int base_sprite_get_height(lua_State* L) {
	auto* base_sprite = lua_check_base_sprite(L, 1);
	lua_pushinteger(L, base_sprite->Height);
	return 1;
}

int base_sprite_get_key_x(lua_State* L) {
	auto* base_sprite = lua_check_base_sprite(L, 1);
	lua_pushinteger(L, base_sprite->KeyX);
	return 1;
}
int base_sprite_get_key_y(lua_State* L) {
	auto* base_sprite = lua_check_base_sprite(L, 1);
	lua_pushinteger(L, base_sprite->KeyY);
	return 1;
}
int base_sprite_get_frame_key_x(lua_State* L) {
	auto* base_sprite = lua_check_base_sprite(L, 1);
	int index = (int)luaL_optinteger(L, 2, -1);
	lua_pushinteger(L, base_sprite->GetFrameKeyX(index));
	return 1;
}
int base_sprite_get_frame_key_y(lua_State* L) {
	auto* base_sprite = lua_check_base_sprite(L, 1);
	int index = (int)luaL_optinteger(L, 2, -1);
	lua_pushinteger(L, base_sprite->GetFrameKeyY(index));
	return 1;
}
int base_sprite_get_frame_width(lua_State*L){
	auto* base_sprite = lua_check_base_sprite(L, 1);
	int index = (int)luaL_optinteger(L, 2, -1);
	lua_pushinteger(L, base_sprite->GetFrameWidth(index));
	return 1;
}
int base_sprite_get_frame_height(lua_State*L){
	auto* base_sprite = lua_check_base_sprite(L, 1);
	int index = (int)luaL_optinteger(L, 2, -1);
	lua_pushinteger(L, base_sprite->GetFrameHeight(index));
	return 1;
}


int base_sprite_get_play_time(lua_State* L) {
	auto* base_sprite = lua_check_base_sprite(L, 1);
	lua_pushnumber(L, base_sprite->PlayTime);
	return 1;
}

int base_sprite_get_dir_cnt(lua_State* L) {
	auto* base_sprite = lua_check_base_sprite(L, 1);
	lua_pushinteger(L, base_sprite->GroupCount);
	return 1;
}
int base_sprite_get_total_frames(lua_State* L) {
	auto* base_sprite = lua_check_base_sprite(L, 1);
	lua_pushinteger(L, base_sprite->TotalFrames);
	return 1;
}
int base_sprite_get_current_frame(lua_State* L) {
	auto* base_sprite = lua_check_base_sprite(L, 1);
	lua_pushinteger(L, base_sprite->CurrentFrame);
	return 1;
}
int base_sprite_set_current_frame(lua_State* L) {
	auto* base_sprite = lua_check_base_sprite(L, 1);
	int frame = (int)lua_tointeger(L, 2);
	base_sprite->CurrentFrame = frame;
	return 0;
}

int base_sprite_get_group_frame_count(lua_State* L) {
	auto* base_sprite = lua_check_base_sprite(L, 1);
	lua_pushinteger(L, base_sprite->GroupFrameCount);
	return 1;
}
int base_sprite_get_group_count(lua_State* L) {
	auto* base_sprite = lua_check_base_sprite(L, 1);
	lua_pushinteger(L, base_sprite->GroupCount);
	return 1;
}

int base_sprite_enable_drag(lua_State* L) {
	auto* base_sprite = lua_check_base_sprite(L, 1);
	bool enable = lua_toboolean(L, 2);
	base_sprite->EnableDrag(enable);
	return 0;
}


int base_sprite_export(lua_State* L) {
	auto* base_sprite = lua_check_base_sprite(L, 1);
	const char* dir = lua_tostring(L, 2);
	const char* prefix = lua_tostring(L, 3);
	int start = base_sprite->CurrentFrame / base_sprite->GroupFrameCount;
	for (int i = start; i < start + base_sprite->GroupFrameCount; i++) {
		std::string filename(dir);
		filename = filename + prefix + std::to_string(i) + ".tga";
		base_sprite->m_pSprite->SaveImage(filename.c_str(), i);
	}

	return 0;
}

int base_sprite_destroy(lua_State* L) {
	BaseSprite** ptr = (BaseSprite**)lua_touserdata(L, 1);
	delete *ptr;
	*ptr = nullptr;
	return 0;
}

luaL_Reg MT_BASE_SPRITE[] = {
	{ "Update",base_sprite_update },
{ "Draw",base_sprite_draw },
{ "SetPos", base_sprite_set_pos },
{ "GetPos", base_sprite_get_pos },
{ "SetDir",base_sprite_set_dir },
{ "GetDir",base_sprite_get_dir },
{ "Stop",base_sprite_stop},
{ "Play",base_sprite_start},
{ "SetFrameInterval",base_sprite_set_frame_interval },
{ "GetFrameInterval",base_sprite_get_frame_interval },
{ "GetWidth", base_sprite_get_width },
{ "GetHeight", base_sprite_get_height },
{ "GetKeyX", base_sprite_get_key_x },
{ "GetKeyY", base_sprite_get_key_y },
{ "GetFrameKeyX", base_sprite_get_frame_key_x },
{ "GetFrameKeyY", base_sprite_get_frame_key_y },
{ "GetFrameHeight", base_sprite_get_frame_height },
{ "GetFrameWidth", base_sprite_get_frame_width},
{ "GetPlayTime", base_sprite_get_play_time },
{ "GetDirCnt", base_sprite_get_dir_cnt },
{ "GetTotalFrames", base_sprite_get_total_frames },
{ "GetCurrentFrame", base_sprite_get_current_frame },
{ "SetCurrentFrame", base_sprite_set_current_frame },
{ "GetGroupFrameCount", base_sprite_get_group_frame_count },
{ "GetGroupCount", base_sprite_get_group_count },
{ "EnableDrag", base_sprite_enable_drag },
{ "Export", base_sprite_export },
{ "Destroy", base_sprite_destroy },
{ NULL, NULL }
};

void lua_push_base_sprite(lua_State*L, BaseSprite* sprite)
{
	BaseSprite** ptr = (BaseSprite**)lua_newuserdata(L, sizeof(BaseSprite*));
	*ptr = sprite;
	if (luaL_newmetatable(L, "MT_BASE_SPRITE")) {
		luaL_setfuncs(L, MT_BASE_SPRITE, 0);
		lua_pushvalue(L, -1);
		lua_setfield(L, -2, "__index");
	}
	lua_setmetatable(L, -2);
}

int base_sprite_create(lua_State*L)
{
	uint32_t pack = (uint32_t)lua_tointeger(L, 1);
	uint32_t wasid = (uint32_t)lua_tointeger(L, 2);
	BaseSprite** ptr = (BaseSprite**)lua_newuserdata(L, sizeof(BaseSprite));
	*ptr = new BaseSprite(pack, wasid);
	if (luaL_newmetatable(L, "MT_BASE_SPRITE")) {
		luaL_setfuncs(L, MT_BASE_SPRITE, 0);
		lua_pushvalue(L, -1);
		lua_setfield(L, -2, "__index");
	}
	lua_setmetatable(L, -2);
	return 1;
}

void luaopen_sprite(lua_State* L)
{
	script_system_register_luac_function(L, base_sprite_create);
}

