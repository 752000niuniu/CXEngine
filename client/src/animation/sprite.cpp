#include "animation/sprite.h"
#include <NESupport.h>
#include "texture_manager.h"
#include "resource_manager.h"
#include "window.h"
#include "sprite_renderer.h"
#include "input_manager.h"
#include "logger.h"
#include "cxmath.h"
#include "actor/actor_manager.h"
#include "graphics/ui_renderer.h"

String UtilsGetFramePath(Sprite* m_pSprite, int index)
{
	if (m_pSprite)
	{
		return m_pSprite->Path + std::string("/" + std::to_string(index));
	}
	return "";
}

Texture* UtilsGetFrameTexture(Sprite* m_pSprite, int index)
{
	if (!m_pSprite)return nullptr;
	if (index >= m_pSprite->GroupFrameCount * m_pSprite->GroupCount)return nullptr;
	auto path = UtilsGetFramePath(m_pSprite, index);
	auto& frame = m_pSprite->Frames[index];
	return TextureManager::GetInstance()->LoadTexture(path, frame.Width, frame.Height, true, (uint8_t*)frame.Src.data());
}

BaseSprite::BaseSprite(uint64_t resoureID, std::vector<PalSchemePart>* patMatrix )
	:ResID(resoureID)
{
	if (resoureID == 0) { m_pSprite = nullptr; return; }
	PalSpriteInfo* spritInfo = nullptr;
	if (patMatrix) {
		spritInfo = RESOURCE_MANAGER_INSTANCE->LoadSprite(resoureID, patMatrix);
	}
	else {
		spritInfo = RESOURCE_MANAGER_INSTANCE->LoadSprite(resoureID, nullptr);
	}
	

	m_pSprite = spritInfo->sprite;
	Width = m_pSprite->Width;
	Height = m_pSprite->Height;
	KeyX = m_pSprite->KeyX;
	KeyY = m_pSprite->KeyY;
	TotalFrames = m_pSprite->GroupCount*m_pSprite->GroupFrameCount;
	GroupFrameCount = m_pSprite->GroupFrameCount;
	GroupCount = m_pSprite->GroupCount;
	m_pSprite->Path = std::to_string(resoureID) + "/" + std::to_string(spritInfo->pati);

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

BaseSprite::BaseSprite(uint32_t pkg, uint32_t wasID, std::vector<PalSchemePart>* patMatrix ) :BaseSprite(RESOURCE_MANAGER_INSTANCE->EncodeWAS(pkg, wasID),patMatrix) {}

BaseSprite::~BaseSprite()
{
	INPUT_MANAGER_INSTANCE->UnRegisterView(this);
	m_pSprite = nullptr;
}

NE::Sprite::Sequence* BaseSprite::GetFrame(int index /*= -1*/)
{
	if (!m_pSprite)return nullptr;
	if (index == -1)index = CurrentFrame;
	if (index >= m_pSprite->Frames.size())return 0;
	return &m_pSprite->Frames[index];
}
int BaseSprite::GetFrameKeyX(int index /*= -1*/)
{
	if (!m_pSprite)return 0;
	if (index == -1)index = CurrentFrame;
	int frame = Dir * GroupFrameCount + index;
	if (frame >= m_pSprite->Frames.size())return 0;
	return m_pSprite->Frames[frame].KeyX;
}

int BaseSprite::GetFrameKeyY(int index /*= -1*/)
{
	if (!m_pSprite)return 0;
	if (index == -1)index = CurrentFrame;
	int frame = Dir * GroupFrameCount + index;
	if (frame >= m_pSprite->Frames.size())return 0;
	return m_pSprite->Frames[frame].KeyY;
}

int BaseSprite::GetFrameWidth(int index /*= -1*/)
{
	if (!m_pSprite)return 0;
	if (index == -1)index = CurrentFrame;
	int frame = Dir * GroupFrameCount + index;
	if (frame >= m_pSprite->Frames.size())return 0;
	return m_pSprite->Frames[frame].Width;
}


int BaseSprite::GetFrameHeight(int index /*= -1*/)
{
	if (!m_pSprite)return 0;
	if (index == -1)index = CurrentFrame;
	int frame = Dir * GroupFrameCount + index;
	if (frame >= m_pSprite->Frames.size())return 0;
	return m_pSprite->Frames[frame].Height;
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
	return Bound{ Pos.x - KeyX, Pos.x - KeyX + Width,
		Pos.y - KeyY,Pos.y - KeyY + Height };
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

Animation::Animation(uint64_t resoureID /*= 0*/, std::vector<PalSchemePart>* patMatrix ) :BaseSprite(resoureID, patMatrix)
{
	m_Visible = true;
	m_State = ANIMATION_PLAY;
	m_LoopCount = 0;
	m_bFrameUpdate = false;
	m_bGroupEndUpdate = false;
	int max_dx = 0;
	int max_frame = 0;
	for (int i = std::max(GroupFrameCount / 2 - 1, 0); i < GroupFrameCount - 1; i++) {
		int dx =(int)std::pow((m_pSprite->Frames[i].KeyX - m_pSprite->Frames[i + 1].KeyX), 2);// + std::abs(m_pSprite->Frames[i].width*m_pSprite->Frames[i].height - m_pSprite->Frames[i + 1].width*m_pSprite->Frames[i + 1].height);
		if (max_dx < dx) {
			max_dx = dx;
			max_frame = i + 1;
		}
	}
	AttackKeyFrame = max_frame;
	m_bTranslate = false;
	m_TranslatePos.x = Pos.x;
	m_TranslatePos.y = Pos.y;
	m_Velocity.x = 0;
	m_Velocity.y = 0;
	m_bLockFrame = false;
	m_LockFrame = CurrentFrame;
	m_bLoop = true;
}

Animation::Animation(uint32_t pkg, uint32_t wasID, std::vector<PalSchemePart>* patMatrix)
	:Animation(RESOURCE_MANAGER_INSTANCE->EncodeWAS(pkg, wasID),patMatrix)
{

}

void Animation::SetLoop(int loop)
{
	if (loop < 0) {
		m_LoopCount = 0;
		m_State = ANIMATION_PLAY;
		m_bLoop = false;
	}
	else {
		m_LoopCount = loop;
		m_bLoop = true;
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

void Animation::Translate(CXPos pos, int duration)
{
	m_bTranslate = true;
	m_Duration = duration/1000.f;
	m_TranslatePos = pos;
	m_Velocity = CXPos((m_TranslatePos.x - Pos.x) / m_Duration, (m_TranslatePos.y - Pos.y) / m_Duration);
}

void Animation::LockFrame(int frame)
{
	m_bLockFrame = true;
	m_LockFrame = frame;
}

void Animation::UnLockFrame()
{
	m_bLockFrame = false;
	m_LockFrame = 0;
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
				if(m_bLoop){
					CurrentFrame = 0;
					if (m_LoopCount > 0) {
						m_LoopCount = m_LoopCount - 1;
						if (m_LoopCount == 0) {
							m_bLoop = false;
							m_State = ANIMATION_STOP;
						}
					}
				}
				else {
					CurrentFrame = CurrentFrame - 1;
					m_State = ANIMATION_STOP;
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
	if (m_bLockFrame) {
		CurrentFrame = m_LockFrame;
	}
	if (m_State != ANIMATION_STOP) {
		if (m_bTranslate) {
			float dist = std::pow(m_Velocity.x*dt, 2) + std::pow(m_Velocity.y*dt, 2);
			if (GMath::Astar_GetDistanceSquare(Pos.x, Pos.y, m_TranslatePos.x, m_TranslatePos.y) > dist) {
				Pos.x = Pos.x + m_Velocity.x*dt;
				Pos.y = Pos.y + m_Velocity.y*dt;
			}
			else {
				Pos.x = m_TranslatePos.x;
				Pos.y = m_TranslatePos.y;
				m_bTranslate = false;
			}
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
		auto& frame = m_pSprite->Frames[Dir*GroupFrameCount + CurrentFrame];
		SPRITE_RENDERER_INSTANCE->DrawFrameSprite(texture,
			glm::vec2(Pos.x - frame.KeyX, Pos.y - frame.KeyY),
			glm::vec2(frame.Width, frame.Height), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));

	}
}

void Animation::Pause(int ms)
{
	m_PauseTime = ms;
	m_PreviousState = m_State;
	m_State = ANIMATION_PAUSE;
}




BeatNumber::BeatNumber() :
	m_HitAnim(MISCWDF, 0x30F737D8),
	m_HealAnim(MISCWDF, 0x3CF8F9FE),
	m_Number(0),
	m_Pos(0, 0),
	m_PlayTime(0),
	m_BeatTime(0.2f),
	m_AdvanceX(12.f),
	m_BeatHeights(2.5f),
	m_Visible(false),
	m_PauseTime(0.2f)
{
	
}

void BeatNumber::Update()
{
	if (!m_Visible)return;
	float dt = WINDOW_INSTANCE->GetDeltaTime();
	m_PlayTime += dt;
	if(m_bBeat){
		for (int i = 0; i < m_Digits.size(); i++)
		{
			Digit& dig = m_Digits[i];
			float start_time = i * (m_BeatTime / m_BeatHeights);
			if (m_PlayTime > start_time) {
				float dur = m_PlayTime - start_time;
				dig.y = (m_HitAnim.m_pSprite->Height * m_BeatHeights / m_BeatTime)*dur;
				if (dig.y > m_HitAnim.m_pSprite->Height * m_BeatHeights * 2) {
					dig.y = 0;
					if (i == m_Digits.size() - 1) {
						m_bBeat = false;
						m_PlayTime = -m_PauseTime;
					}
				}
				else if (dig.y > m_HitAnim.m_pSprite->Height*m_BeatHeights) {
					dig.y = m_HitAnim.m_pSprite->Height * m_BeatHeights * 2 - dig.y;
				}
			}
		}
	}
	else {
		if(m_PlayTime>=0){
			m_PlayTime = 0;
			m_Visible = false;
		}
	}
}

void BeatNumber::Draw()
{
	if (!m_Visible)return;

	float px = m_Pos.x - m_Digits.size() * m_AdvanceX / 2.f;
	for (int i = 0; i < m_Digits.size(); i++)
	{
		Digit& dig = m_Digits[i];
		float x = px + i*m_AdvanceX;
		float y = m_Pos.y - dig.y;
		auto* texture = UtilsGetFrameTexture(m_HitAnim.m_pSprite, dig.digit);
		if (texture)
		{
			auto& frame = m_HitAnim.m_pSprite->Frames[dig.digit];
			SPRITE_RENDERER_INSTANCE->DrawFrameSprite(texture,
				glm::vec2(x - frame.KeyX, y - frame.KeyY),
				glm::vec2(frame.Width, frame.Height), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
		}
	}
}

void BeatNumber::SetPos(float x, float y)
{
	m_Pos.x = x;
	m_Pos.y = y;
}

void BeatNumber::SetNumber(int num)
{
	m_Number = num;
	m_Digits.clear();
	int i = 0;
	do {
		int mod = num % 10;
		Digit d;
		d.y = 0;
		d.digit = mod;
		m_Digits.push_front(d);
		num = num / 10;
	} while (num);
}

void BeatNumber::Beat()
{
	m_Visible = true;
	m_PlayTime = -m_PauseTime;
	m_bBeat = true;
}


AnimationManager::AnimationManager()
{
	m_Animations.clear();
	m_BeatNumbers.clear();
}

AnimationManager::~AnimationManager()
{

}

void AnimationManager::AddAnimation(Animation* animation)
{
	m_Animations.push_back(animation);
}

void AnimationManager::AddBeatNumber(BeatNumber* bn)
{
	m_BeatNumbers.push_back(bn);
}

void AnimationManager::Update()
{
	{
		std::vector<Animation*> tmpSet(m_Animations.begin(), m_Animations.end());
		m_Animations.clear();
		for(auto& it : tmpSet){
			it->Update();
			if (it->GetState() == ANIMATION_STOP) {
				delete it;
			}
			else {
				m_Animations.push_back(it);
			}
		}
	}
	{
		std::vector<BeatNumber*> tmpSet(m_BeatNumbers.begin(), m_BeatNumbers.end());
		m_BeatNumbers.clear();
		for (auto& it : tmpSet) {
			it->Update();
			if (it->GetVisible() == false) {
				delete it;
			}
			else {
				m_BeatNumbers.push_back(it);
			}
		}
	}
}

void AnimationManager::Draw()
{
	for (auto& it : m_Animations) {
		it->Draw();
	}
	for (auto it = m_BeatNumbers.begin(); it != m_BeatNumbers.end(); it++)
	{
		(*it)->Draw();
	}
}

BaseSprite* lua_check_base_sprite(lua_State* L, int index)
{
	return lua_check_pointer<BaseSprite>(L, index);
}


Animation* lua_check_animation(lua_State*L, int index)
{
	return lua_check_pointer<Animation>(L, index);
}

int base_sprite_update(lua_State* L) {
	auto* sp = lua_check_base_sprite(L, 1);
	sp->Update();
	return 0;
}

int base_sprite_draw(lua_State* L) {
	auto* sp = lua_check_base_sprite(L, 1);
	sp->Draw();
	return 0;
}

int base_sprite_set_pos(lua_State* L) {
	auto* sp = lua_check_base_sprite(L, 1);
	auto x = (float)lua_tonumber(L, 2);
	auto y = (float)lua_tonumber(L, 3);
	sp->Pos.x = x;
	sp->Pos.y = y;
	return 0;
}

int base_sprite_get_pos(lua_State* L) {
	auto* sp = lua_check_base_sprite(L, 1);
	lua_pushnumber(L, sp->Pos.x);
	lua_pushnumber(L, sp->Pos.y);
	return 2;
}

int base_sprite_set_dir(lua_State* L) {
	auto* sp = lua_check_base_sprite(L, 1);
	auto dir = (int)lua_tointeger(L, 2);
	sp->Dir = dir;
	return 0;
}

int base_sprite_get_dir(lua_State* L) {
	auto* sp = lua_check_base_sprite(L, 1);
	lua_pushinteger(L, sp->Dir);
	return 1;
}


int base_sprite_set_frame_interval(lua_State* L) {
	auto* sp = lua_check_base_sprite(L, 1);
	auto interval = (float)lua_tonumber(L, 2);
	sp->FrameInterval = interval;
	sp->CurrentFrame = 0;
	sp->PlayTime = 0.f;
	return 0;
}

int base_sprite_get_frame_interval(lua_State* L) {
	auto* sp = lua_check_base_sprite(L, 1);
	lua_pushnumber(L, sp->FrameInterval);
	return 1;
}

int base_sprite_get_width(lua_State* L) {
	auto* sp = lua_check_base_sprite(L, 1);
	lua_pushinteger(L, sp->Width);
	return 1;
}

int base_sprite_get_height(lua_State* L) {
	auto* sp = lua_check_base_sprite(L, 1);
	lua_pushinteger(L, sp->Height);
	return 1;
}


int base_sprite_set_width(lua_State* L) {
	auto* sp = lua_check_base_sprite(L, 1);
	float w = (float)lua_tonumber(L, 2);
	sp->Width = (int)w;
	return 0;
}

int base_sprite_set_height(lua_State* L) {
	auto* sp = lua_check_base_sprite(L, 1);
	float h = (float)lua_tonumber(L, 2);
	sp->Height = (int)h;
	return 0;
}

int base_sprite_get_KeyX(lua_State* L) {
	auto* sp = lua_check_base_sprite(L, 1);
	lua_pushinteger(L, sp->KeyX);
	return 1;
}
int base_sprite_get_KeyY(lua_State* L) {
	auto* sp = lua_check_base_sprite(L, 1);
	lua_pushinteger(L, sp->KeyY);
	return 1;
}
int base_sprite_get_frame_KeyX(lua_State* L) {
	auto* sp = lua_check_base_sprite(L, 1);
	int index = (int)luaL_optinteger(L, 2, -1);
	lua_pushinteger(L, sp->GetFrameKeyX(index));
	return 1;
}
int base_sprite_get_frame_KeyY(lua_State* L) {
	auto* sp = lua_check_base_sprite(L, 1);
	int index = (int)luaL_optinteger(L, 2, -1);
	lua_pushinteger(L, sp->GetFrameKeyY(index));
	return 1;
}
int base_sprite_get_frame_width(lua_State* L) {
	auto* sp = lua_check_base_sprite(L, 1);
	int index = (int)luaL_optinteger(L, 2, -1);
	lua_pushinteger(L, sp->GetFrameWidth(index));
	return 1;
}
int base_sprite_get_frame_height(lua_State* L) {
	auto* sp = lua_check_base_sprite(L, 1);
	int index = (int)luaL_optinteger(L, 2, -1);
	lua_pushinteger(L, sp->GetFrameHeight(index));
	return 1;
}

int base_sprite_get_play_time(lua_State* L) {
	auto* sp = lua_check_base_sprite(L, 1);
	lua_pushnumber(L, sp->PlayTime);
	return 1;
}

int base_sprite_get_dir_cnt(lua_State* L) {
	auto* sp = lua_check_base_sprite(L, 1);
	lua_pushinteger(L, sp->GroupCount);
	return 1;
}
int base_sprite_get_total_frames(lua_State* L) {
	auto* sp = lua_check_base_sprite(L, 1);
	lua_pushinteger(L, sp->TotalFrames);
	return 1;
}
int base_sprite_get_current_frame(lua_State* L) {
	auto* sp = lua_check_base_sprite(L, 1);
	lua_pushinteger(L, sp->CurrentFrame);
	return 1;
}
int base_sprite_set_current_frame(lua_State* L) {
	auto* sp = lua_check_base_sprite(L, 1);
	int frame = (int)lua_tointeger(L, 2);
	sp->CurrentFrame = frame;
	return 0;
}

int base_sprite_get_group_frame_count(lua_State* L) {
	auto* sp = lua_check_base_sprite(L, 1);
	lua_pushinteger(L, sp->GroupFrameCount);
	return 1;
}
int base_sprite_get_group_count(lua_State* L) {
	auto* sp = lua_check_base_sprite(L, 1);
	lua_pushinteger(L, sp->GroupCount);
	return 1;
}

int base_sprite_enable_drag(lua_State* L) {
	auto* sp = lua_check_base_sprite(L, 1);
	bool enable = lua_toboolean(L, 2);
	sp->EnableDrag(enable);
	return 0;
}

int base_sprite_export(lua_State* L) {
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


int base_sprite_export_was(lua_State* L) {
	auto* sp = lua_check_base_sprite(L, 1);
	const char* path = lua_tostring(L, 2);
	RESOURCE_MANAGER_INSTANCE->ExportWas(sp->ResID, path);
	return 0;
}


int base_sprite_destroy(lua_State* L) {
	auto* sp = lua_check_base_sprite(L, 1);
	delete sp;
	return 0;
}

luaL_Reg MT_BASE_SPRITE[] = {
	{ "Update",base_sprite_update },
{ "Draw",base_sprite_draw },
{ "SetPos", base_sprite_set_pos },
{ "GetPos", base_sprite_get_pos },
{ "SetDir",base_sprite_set_dir },
{ "GetDir",base_sprite_get_dir },
{ "SetFrameInterval",base_sprite_set_frame_interval },
{ "GetFrameInterval",base_sprite_get_frame_interval },
{ "GetWidth", base_sprite_get_width },
{ "GetHeight", base_sprite_get_height },
{ "SetWidth", base_sprite_set_width },
{ "SetHeight", base_sprite_set_height },
{ "GetKeyX", base_sprite_get_KeyX },
{ "GetKeyY", base_sprite_get_KeyY },
{ "GetFrameKeyX", base_sprite_get_frame_KeyX },
{ "GetFrameKeyY", base_sprite_get_frame_KeyY },
{ "GetFrameHeight", base_sprite_get_frame_height },
{ "GetFrameWidth", base_sprite_get_frame_width },
{ "GetPlayTime", base_sprite_get_play_time },
{ "GetDirCnt", base_sprite_get_dir_cnt },
{ "GetTotalFrames", base_sprite_get_total_frames },
{ "GetCurrentFrame", base_sprite_get_current_frame },
{ "SetCurrentFrame", base_sprite_set_current_frame },
{ "GetGroupFrameCount", base_sprite_get_group_frame_count },
{ "GetGroupCount", base_sprite_get_group_count },
{ "EnableDrag", base_sprite_enable_drag },
{ "Export", base_sprite_export },
{ "ExportWas", base_sprite_export_was },
{ "Destroy", base_sprite_destroy },
{ NULL, NULL }
};


int animation_pause(lua_State* L) {
	auto* animation = lua_check_animation(L, 1);
	int ms = (int)lua_tointeger(L, 2);
	animation->Pause(ms);
	return 0;
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

int animation_lock_frame(lua_State* L) {
	auto* animation = lua_check_animation(L, 1);
	int frame = (int)luaL_optinteger(L, 2, animation->CurrentFrame);
	animation->LockFrame(frame);
	return 0;
}

int animation_unlock_frame(lua_State* L) {
	auto* animation = lua_check_animation(L, 1);
	animation->UnLockFrame();
	return 0;
}

int animation_set_loop(lua_State* L) {
	auto* animation = lua_check_animation(L, 1);
	int loop = (int)lua_tointeger(L, 2);
	animation->SetLoop(loop);
	return 0;
}

int animation_get_visible(lua_State* L) {
	auto* animation = lua_check_animation(L, 1);
	lua_pushinteger(L, animation->GetVisible());
	return 1;
}

int animation_set_visible(lua_State* L) {
	auto* animation = lua_check_animation(L, 1);
	int visible = (int)lua_tointeger(L, 2);
	animation->SetVisible(visible);
	return 0;
}

int animation_translate(lua_State*L) {
	auto* animation = lua_check_animation(L, 1);
	float x = (float)lua_tonumber(L, 2);
	float y = (float)lua_tonumber(L, 3);
	int dur = (int)lua_tointeger(L, 4);
	Pos tpos(animation->Pos.x + x, animation->Pos.y + y);
	animation->Translate(tpos, dur);
	return 0;
}

luaL_Reg MT_ANIMATION[] = {
{ "Pause",animation_pause },
{ "Stop",animation_stop},
{ "Play",animation_start },
{ "LockFrame",animation_lock_frame },
{ "UnLockFrame",animation_unlock_frame },
{ "SetLoop",animation_set_loop },
{ "GetVisible", animation_get_visible },
{ "SetVisible", animation_set_visible },
{ "Translate", animation_translate },
{ NULL, NULL }
};


luaL_Reg MT_NE_IAMGEVIEW[] = {
	{NULL,NULL}
};

void lua_push_base_sprite(lua_State* L, BaseSprite* sprite)
{
	lua_push_pointer(L, sprite);
	if (luaL_newmetatable(L, "MT_BASE_SPRITE")) {
		luaL_setfuncs(L, MT_BASE_SPRITE, 0);
		lua_pushvalue(L, -1);
		lua_setfield(L, -2, "__index");
	}
	lua_setmetatable(L, -2);
}

void lua_push_animation(lua_State*L, Animation* sprite)
{
	lua_push_pointer(L, sprite);
	if (luaL_newmetatable(L, "MT_BASE_SPRITE")) {
		luaL_setfuncs(L, MT_BASE_SPRITE, 0);
		luaL_setfuncs(L, MT_ANIMATION, 0);
		lua_pushvalue(L, -1);
		lua_setfield(L, -2, "__index");
	}
	lua_setmetatable(L, -2);
}

int animation_create(lua_State* L)
{
	uint32_t pack = (uint32_t)lua_tointeger(L, 1);
	uint32_t wasid = (uint32_t)lua_tointeger(L, 2);

	Animation** ptr = (Animation * *)lua_newuserdata(L, sizeof(Animation));
	*ptr = new Animation(pack, wasid);
	if (luaL_newmetatable(L, "MT_ANIMATION")) {
		luaL_setfuncs(L, MT_BASE_SPRITE, 0);
		luaL_setfuncs(L, MT_ANIMATION, 0);
		lua_pushvalue(L, -1);
		lua_setfield(L, -2, "__index");
	}
	lua_setmetatable(L, -2);
	return 1;
}



int base_sprite_get_metatable(lua_State* L) {
	if (luaL_newmetatable(L, "MT_BASE_SPRITE")) {
		luaL_setfuncs(L, MT_BASE_SPRITE, 0);
		lua_pushvalue(L, -1);
		lua_setfield(L, -2, "__index");
	}
	return 1;
}

int beat_number_update(lua_State*L) {
	auto* bn = lua_check_pointer<BeatNumber>(L, 1);
	bn->Update();
	return 0;
}

int beat_number_draw(lua_State*L){
	auto* bn = lua_check_pointer<BeatNumber>(L, 1);
	bn->Draw();
	return 0;
}

int beat_number_beat(lua_State*L) {
	auto* bn = lua_check_pointer<BeatNumber>(L, 1);
	bn->Beat();
	return 0;
}

int beat_number_set_pos(lua_State*L) {
	auto* bn = lua_check_pointer<BeatNumber>(L, 1);
	float x = (float)lua_tonumber(L, 2);
	float y = (float)lua_tonumber(L, 3);
	bn->SetPos(x, y);
	return 0;
}

int beat_number_set_number(lua_State*L) {
	auto* bn = lua_check_pointer<BeatNumber>(L, 1);
	int  num = (int)lua_tointeger(L, 2);
	bn->SetNumber(num);
	return 0;
}


luaL_Reg MT_BEAT_NUMBER[] = {
	{ "Update",beat_number_update },
	{ "Draw",beat_number_draw },
	{ "Beat",beat_number_beat },
	{ "SetPos",beat_number_set_pos},
	{ "SetNumber",beat_number_set_number },
};

int beat_number_create(lua_State*L)
{
	lua_push_pointer(L, new BeatNumber());
	if (luaL_newmetatable(L, "MT_BEAT_NUMBER")) {
		luaL_setfuncs(L, MT_BEAT_NUMBER, 0);
		lua_pushvalue(L, -1);
		lua_setfield(L, -2, "__index");
	}
	lua_setmetatable(L, -2);
	return 1;
}




void luaopen_sprite(lua_State* L)
{
	script_system_register_luac_function(L, animation_create);
	script_system_register_luac_function(L, base_sprite_get_metatable);
	script_system_register_luac_function(L, beat_number_create);
	
}

