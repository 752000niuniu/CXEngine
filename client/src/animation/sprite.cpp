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
	if (resoureID == 0) { m_pSprite = nullptr; return; }
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
	bEnableDrag = false;
}

BaseSprite::BaseSprite(uint32_t pkg, uint32_t wasID) :BaseSprite(RESOURCE_MANAGER_INSTANCE->EncodeWAS(pkg, wasID)) {}

BaseSprite::~BaseSprite()
{
	INPUT_MANAGER_INSTANCE->UnRegisterView(this);
	m_pSprite = nullptr;
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
	m_Visible = true;
	m_State = ANIMATION_PLAY;
	m_LoopCount = 0;
	m_bFrameUpdate = false;
	m_bGroupEndUpdate = false;
	int max_dx = 0;
	int max_frame = 0;
	for (int i = std::max(GroupFrameCount / 2 - 1, 0); i < GroupFrameCount - 1; i++) {
		int dx = std::pow((m_pSprite->mFrames[i].key_x - m_pSprite->mFrames[i + 1].key_x), 2);// + std::abs(m_pSprite->mFrames[i].width*m_pSprite->mFrames[i].height - m_pSprite->mFrames[i + 1].width*m_pSprite->mFrames[i + 1].height);
		if (max_dx < dx) {
			max_dx = dx;
			max_frame = i + 1;
		}
	}
	AttackKeyFrame = max_frame;
}

Animation::Animation(uint32_t pkg, uint32_t wasID)
	:Animation(RESOURCE_MANAGER_INSTANCE->EncodeWAS(pkg, wasID))
{

}
void Animation::SetLoop(int loop)
{
	if (loop < 0) {
		m_LoopCount = 0;
		m_State = ANIMATION_PLAY;
	}
	else {
		m_LoopCount = loop;
		m_State = ANIMATION_LOOP;
	}
}
void Animation::Reset()
{
	PlayTime = 0;
	CurrentFrame = 0;
}

void Animation::Stop()
{
	m_PreviousState = m_State;
	m_State = ANIMATION_STOP;
}

void Animation::Play()
{
	m_PreviousState = m_State;
	m_State = ANIMATION_PLAY;
}

void Animation::Replay()
{
	Reset();
	Play();
	m_Visible = true;
}

void Animation::AddFrameCallback(int frame, std::function<void()> callback)
{
	m_Callbacks[frame] = callback;
}

void Animation::Update()
{
	if (!m_pSprite)return;
	
	m_bGroupEndUpdate = false;
	m_bFrameUpdate = false;
	float dt = WINDOW_INSTANCE->GetDeltaTime();
	if (m_State == ANIMATION_PLAY) {
		PlayTime = PlayTime + dt;
		if (PlayTime >= FrameInterval)
		{
			m_bFrameUpdate = true;
			PlayTime = (PlayTime - std::floor(PlayTime / FrameInterval)*FrameInterval);
			CurrentFrame = CurrentFrame + 1;
			if (m_Callbacks[CurrentFrame]) {
				m_Callbacks[CurrentFrame]();
			}
			if (CurrentFrame >= GroupFrameCount) {
				m_bGroupEndUpdate = true;
				CurrentFrame = CurrentFrame - 1;
				m_State = ANIMATION_STOP;
			}
		}
	}
	else if (m_State == ANIMATION_LOOP) {
		PlayTime = PlayTime + dt;
		if (PlayTime >= FrameInterval)
		{
			m_bFrameUpdate = true;
			PlayTime = (PlayTime - std::floor(PlayTime / FrameInterval)*FrameInterval);
			CurrentFrame = CurrentFrame + 1;
			if (m_Callbacks[CurrentFrame]) {
				m_Callbacks[CurrentFrame]();
			}
			if (CurrentFrame >= GroupFrameCount) {
				m_bGroupEndUpdate = true;
				CurrentFrame = 0;
				if (m_LoopCount > 0) {
					m_LoopCount = m_LoopCount - 1;
					if (m_LoopCount == 0) {
						m_State = ANIMATION_STOP;
					}
				}
			}
		}
	}
	else if (m_State == ANIMATION_STOP) {
		
	}
	else if (m_State == ANIMATION_PAUSE) {
		int ms = (int)(dt * 1000);
		m_PauseTime = m_PauseTime - ms;
		if (m_PauseTime <= 0) {
			m_State = m_PreviousState;
		}
	}
}

void Animation::Draw()
{
	if (!m_pSprite)return;
	if (!m_Visible)return;

	auto* texture = UtilsGetFrameTexture(m_pSprite, Dir*GroupFrameCount + CurrentFrame);
	if (texture)
	{
		auto& frame = m_pSprite->mFrames[Dir*GroupFrameCount + CurrentFrame];
		SPRITE_RENDERER_INSTANCE->DrawFrameSprite(texture,
			glm::vec2(Pos.x - frame.key_x, Pos.y - frame.key_y),
			glm::vec2(frame.width, frame.height), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));

	}
}

void Animation::Pause(int ms)
{
	m_PauseTime = ms;
	m_PreviousState = m_State;
	m_State = ANIMATION_PAUSE;
}

Animation* lua_check_animation(lua_State*L, int index)
{
	Animation** ptr = (Animation**)lua_touserdata(L, index);
	return *ptr;
}

int animation_update(lua_State* L) {
	auto* animation = lua_check_animation(L, 1);
	animation->Update();
	return 0;
}

int animation_draw(lua_State* L) {
	auto* animation = lua_check_animation(L, 1);
	animation->Draw();
	return 0;
}

int animation_set_pos(lua_State* L) {
	auto* animation = lua_check_animation(L, 1);
	auto x = (float)lua_tonumber(L, 2);
	auto y = (float)lua_tonumber(L, 3);
	animation->Pos.x = x;
	animation->Pos.y = y;
	return 0;
}

int animation_get_pos(lua_State* L) {
	auto* animation = lua_check_animation(L, 1);
	lua_pushnumber(L, animation->Pos.x);
	lua_pushnumber(L, animation->Pos.y);
	return 2;
}

int animation_set_dir(lua_State* L) {
	auto* animation = lua_check_animation(L, 1);
	auto dir = (int)lua_tointeger(L, 2);
	animation->Dir = dir;
	return 0;
}

int animation_get_dir(lua_State* L) {
	auto* animation = lua_check_animation(L, 1);
	lua_pushinteger(L, animation->Dir);
	return 1;
}

int animation_stop(lua_State* L) {
	auto* animation = lua_check_animation(L, 1);
	animation->Stop();
	return 0;
}

int animation_start(lua_State* L) {
	auto* animation = lua_check_animation(L, 1);
	animation->Play();
	return 0;
}


int animation_set_frame_interval(lua_State* L) {
	auto* animation = lua_check_animation(L, 1);
	auto interval = (float)lua_tonumber(L, 2);
	animation->FrameInterval = interval;
	animation->CurrentFrame = 0;
	animation->PlayTime = 0.f;
	return 0;
}

int animation_get_frame_interval(lua_State* L) {
	auto* animation = lua_check_animation(L, 1);
	lua_pushnumber(L, animation->FrameInterval);
	return 1;
}

int animation_get_width(lua_State* L) {
	auto* animation = lua_check_animation(L, 1);
	lua_pushinteger(L, animation->Width);
	return 1;
}

int animation_get_height(lua_State* L) {
	auto* animation = lua_check_animation(L, 1);
	lua_pushinteger(L, animation->Height);
	return 1;
}

int animation_get_key_x(lua_State* L) {
	auto* animation = lua_check_animation(L, 1);
	lua_pushinteger(L, animation->KeyX);
	return 1;
}
int animation_get_key_y(lua_State* L) {
	auto* animation = lua_check_animation(L, 1);
	lua_pushinteger(L, animation->KeyY);
	return 1;
}
int animation_get_frame_key_x(lua_State* L) {
	auto* animation = lua_check_animation(L, 1);
	int index = (int)luaL_optinteger(L, 2, -1);
	lua_pushinteger(L, animation->GetFrameKeyX(index));
	return 1;
}
int animation_get_frame_key_y(lua_State* L) {
	auto* animation = lua_check_animation(L, 1);
	int index = (int)luaL_optinteger(L, 2, -1);
	lua_pushinteger(L, animation->GetFrameKeyY(index));
	return 1;
}
int animation_get_frame_width(lua_State*L){
	auto* animation = lua_check_animation(L, 1);
	int index = (int)luaL_optinteger(L, 2, -1);
	lua_pushinteger(L, animation->GetFrameWidth(index));
	return 1;
}
int animation_get_frame_height(lua_State*L){
	auto* animation = lua_check_animation(L, 1);
	int index = (int)luaL_optinteger(L, 2, -1);
	lua_pushinteger(L, animation->GetFrameHeight(index));
	return 1;
}


int animation_get_play_time(lua_State* L) {
	auto* animation = lua_check_animation(L, 1);
	lua_pushnumber(L, animation->PlayTime);
	return 1;
}

int animation_get_dir_cnt(lua_State* L) {
	auto* animation = lua_check_animation(L, 1);
	lua_pushinteger(L, animation->GroupCount);
	return 1;
}
int animation_get_total_frames(lua_State* L) {
	auto* animation = lua_check_animation(L, 1);
	lua_pushinteger(L, animation->TotalFrames);
	return 1;
}
int animation_get_current_frame(lua_State* L) {
	auto* animation = lua_check_animation(L, 1);
	lua_pushinteger(L, animation->CurrentFrame);
	return 1;
}
int animation_set_current_frame(lua_State* L) {
	auto* animation = lua_check_animation(L, 1);
	int frame = (int)lua_tointeger(L, 2);
	animation->CurrentFrame = frame;
	return 0;
}

int animation_get_group_frame_count(lua_State* L) {
	auto* animation = lua_check_animation(L, 1);
	lua_pushinteger(L, animation->GroupFrameCount);
	return 1;
}
int animation_get_group_count(lua_State* L) {
	auto* animation = lua_check_animation(L, 1);
	lua_pushinteger(L, animation->GroupCount);
	return 1;
}

int animation_enable_drag(lua_State* L) {
	auto* animation = lua_check_animation(L, 1);
	bool enable = lua_toboolean(L, 2);
	animation->EnableDrag(enable);
	return 0;
}


int animation_export(lua_State* L) {
	auto* animation = lua_check_animation(L, 1);
	const char* dir = lua_tostring(L, 2);
	const char* prefix = lua_tostring(L, 3);
	int start = animation->CurrentFrame / animation->GroupFrameCount;
	for (int i = start; i < start + animation->GroupFrameCount; i++) {
		std::string filename(dir);
		filename = filename + prefix + std::to_string(i) + ".tga";
		animation->m_pSprite->SaveImage(filename.c_str(), i);
	}

	return 0;
}

int animation_destroy(lua_State* L) {
	Animation** ptr = (Animation**)lua_touserdata(L, 1);
	delete *ptr;
	*ptr = nullptr;
	return 0;
}

luaL_Reg MT_BASE_SPRITE[] = {
	{ "Update",animation_update },
{ "Draw",animation_draw },
{ "SetPos", animation_set_pos },
{ "GetPos", animation_get_pos },
{ "SetDir",animation_set_dir },
{ "GetDir",animation_get_dir },
{ "Stop",animation_stop},
{ "Play",animation_start},
{ "SetFrameInterval",animation_set_frame_interval },
{ "GetFrameInterval",animation_get_frame_interval },
{ "GetWidth", animation_get_width },
{ "GetHeight", animation_get_height },
{ "GetKeyX", animation_get_key_x },
{ "GetKeyY", animation_get_key_y },
{ "GetFrameKeyX", animation_get_frame_key_x },
{ "GetFrameKeyY", animation_get_frame_key_y },
{ "GetFrameHeight", animation_get_frame_height },
{ "GetFrameWidth", animation_get_frame_width},
{ "GetPlayTime", animation_get_play_time },
{ "GetDirCnt", animation_get_dir_cnt },
{ "GetTotalFrames", animation_get_total_frames },
{ "GetCurrentFrame", animation_get_current_frame },
{ "SetCurrentFrame", animation_set_current_frame },
{ "GetGroupFrameCount", animation_get_group_frame_count },
{ "GetGroupCount", animation_get_group_count },
{ "EnableDrag", animation_enable_drag },
{ "Export", animation_export },
{ "Destroy", animation_destroy },
{ NULL, NULL }
};

void lua_push_animation(lua_State*L, Animation* sprite)
{
	Animation** ptr = (Animation**)lua_newuserdata(L, sizeof(Animation*));
	*ptr = sprite;
	if (luaL_newmetatable(L, "MT_BASE_SPRITE")) {
		luaL_setfuncs(L, MT_BASE_SPRITE, 0);
		lua_pushvalue(L, -1);
		lua_setfield(L, -2, "__index");
	}
	lua_setmetatable(L, -2);
}

int animation_create(lua_State*L)
{
	uint32_t pack = (uint32_t)lua_tointeger(L, 1);
	uint32_t wasid = (uint32_t)lua_tointeger(L, 2);
	Animation** ptr = (Animation**)lua_newuserdata(L, sizeof(Animation));
	*ptr = new Animation(pack, wasid);
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
	script_system_register_luac_function(L, animation_create);
}

ActionAnimation::ActionAnimation(uint64_t resoureID /*= 0*/)
{

}

void ActionAnimation::Update()
{
	if (!m_pSprite)return;
	float dt = WINDOW_INSTANCE->GetDeltaTime();
	
}

void ActionAnimation::Draw()
{

}

AnimationManager::AnimationManager()
{

}

AnimationManager::~AnimationManager()
{

}

void AnimationManager::AddQueue(Animation* animation)
{
	animation->Reset();
//	animation->Play();
	m_Animations.push_back(animation);
}

void AnimationManager::Update()
{
	std::vector<Animation*> updateSet(m_Animations.begin(), m_Animations.end());
	for (auto& it : updateSet) {
		it->Update();
	}
	m_Animations.clear();
	for (auto& it : updateSet) {
		if (it->GetState() != ANIMATION_STOP) {
			m_Animations.push_back(it);
		}
		else {
			delete it;
		}
	}
}

void AnimationManager::Draw()
{
	for (auto& it : m_Animations) {
		it->Draw();
	}
}
