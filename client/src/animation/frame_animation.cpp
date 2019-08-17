#include "frame_animation.h"
#include "engine.h"
#include "profile.h"
#include "window.h"
#include "resource_manager.h"
#include "logger.h"
#include "ui.h"
#include "input_manager.h"

#define  FRAME_TIME_BASE_DEFAULT (1.0f/60*4)


std::map<float,float> bezier_map_array;

int uniform_speed_interpolate(float total_frame_time,int total_frame,float current_time)
{
	int current_frame = (int)std::floor(current_time / total_frame_time * total_frame);
	return current_frame;
}

float bezier_p1_x = 0.17f;
float bezier_p1_y = 0.67f;
float bezier_p2_x = 0.83f;
float bezier_p2_y = 0.67f;
float bezier_total_frame_time = 0.5f;

//CubicBezierCurves B(t) = (1 - t) ^ 3 * P0 + 3 * (1 - t) ^ 2 * t*P1 + 3 * (1 - t)*t ^ 2 * P2 + t ^ 3 * P3
//standard cubit bezier curve  P0 = (0, 0) P3 = (1, 1)
void general_cubic_bezier_curve_interpolate(float t, float p1_x, float p1_y, float p2_x, float p2_y,float& out_x,float& out_y)
{
	if (t == 0.f) 
	{ 
		out_x = 0.f;
		out_y = 0.f;
		return;
	}

	p1_x = 3 * std::powf(1.f - t, 2)*t * p1_x;
	p1_y = 3 * std::powf(1.f - t, 2)*t * p1_y;
	
	p2_x = 3 * (1.f - t)*std::powf(t, 2)*t * p2_x;
	p2_y = 3 * (1.f - t)*std::powf(t, 2)*t * p2_y;

	float p3_x = std::powf(t, 3);
	float p3_y = std::powf(t, 3);

	out_x= p1_x + p2_x + p3_x;
	out_y = p1_y + p2_y + p3_y;
}

void init_bezier_float_array()
{
	bezier_map_array.clear();
	
	for (int i = 0; i <= 101; i++)
	{
		float x, y;
		x = y = 0.f;
		general_cubic_bezier_curve_interpolate(i*(1.0f/101.f), bezier_p1_x, bezier_p1_y, bezier_p2_x, bezier_p2_y, x, y);
		bezier_map_array[x]=y;
	}
}

int uniform_bezier_interpolate(float total_frame_time, int total_frame, float current_time)
{
	if (total_frame_time == 0.f) return 0;
	float t = current_time / total_frame_time;
	float y = 0.f;
	for (auto& it : bezier_map_array)
	{
		if (it.first >t )
		{
			y = it.second;
			break;
		}
	}
	return (int)std::floor(y*total_frame);
}

FrameAnimation::FrameAnimation(uint64_t resoureID)
	:m_ResourceID(resoureID),
	m_TotalFrameCount(0),
	m_CurrentFrame(0),
	m_LastFrame(-1),
	m_CurrentGroup(0),
	m_GroupFrameCount(0),
	m_KeyX(0),
	m_KeyY(0),
	m_Pos({ 0 }),
	m_Width(0),
	m_Height(0),
	m_bLoop(true),
	m_bIsNextFrameRestart(false),
	m_DeltaTime(0.),
	m_bVisible(false),
	m_bCurrentFrameChangedInUpdate(false),
	m_LastNotBlankFrame(false),
	m_FrameTimeBase(FRAME_TIME_BASE_DEFAULT),
	m_FrameTime(m_FrameTimeBase),
	m_pSprite(nullptr)
{
	m_pSprite = RESOURCE_MANAGER_INSTANCE->LoadWASSpriteByID(m_ResourceID);
	if (!m_pSprite) return;
	m_GroupFrameCount = m_pSprite->mFrameSize;
	m_GroupCount = m_pSprite->mGroupSize;
	m_TotalFrameCount = (int)m_pSprite->mFrames.size();
	m_KeyX = m_pSprite->mKeyX;
	m_KeyY = m_pSprite->mKeyY;
	m_Width = m_pSprite->mWidth;
	m_Height = m_pSprite->mHeight;
	m_bVisible = true;
	m_bLoop =true;
}

FrameAnimation::FrameAnimation(uint32_t pkg, uint32_t wasID)
	:FrameAnimation(RESOURCE_MANAGER_INSTANCE->EncodeWAS(pkg, wasID))
{

}
FrameAnimation& FrameAnimation::operator=(const FrameAnimation& rhs)
{
	this->m_TotalFrameCount = rhs.m_TotalFrameCount;
	this->m_CurrentFrame = rhs.m_CurrentFrame;
	this->m_LastFrame = rhs.m_LastFrame;
	this->m_CurrentGroup = rhs.m_CurrentGroup;
	this->m_GroupFrameCount = rhs.m_GroupFrameCount;
	this->m_KeyX = rhs.m_KeyX;
	this->m_KeyY = rhs.m_KeyY;
	this->m_Pos = rhs.m_Pos;
	this->m_Width = rhs.m_Width;
	this->m_Height = rhs.m_Height;
	this->m_bLoop = rhs.m_bLoop;
	this->m_bIsNextFrameRestart = rhs.m_bIsNextFrameRestart;
	this->m_DeltaTime = rhs.m_DeltaTime;
	this->m_bVisible = rhs.m_bVisible;
	this->m_bCurrentFrameChangedInUpdate = rhs.m_bCurrentFrameChangedInUpdate;
	this->m_LastNotBlankFrame = rhs.m_LastNotBlankFrame;
	this->m_FrameTime = rhs.m_FrameTime;
	this->m_FrameTimeBase = rhs.m_FrameTimeBase;
	this->m_pSprite = rhs.m_pSprite;
	this->m_GroupCount = rhs.m_GroupCount;
	return *this;
}

FrameAnimation::~FrameAnimation()
{
	m_pSprite = nullptr;
}

void FrameAnimation::ResetFrameTimeByGroupCount(int groupCount)
{
	if (m_GroupFrameCount == 0)
	{
		m_FrameTime = m_FrameTimeBase;
	}
	else
	{
		m_FrameTime = m_FrameTimeBase * groupCount / m_GroupFrameCount;
	}
}

void FrameAnimation::SetFrameTimeBase(float base)
{
	m_FrameTimeBase = base;
	m_FrameTime = m_FrameTimeBase;
}

String FrameAnimation::GetFramePath(int index)
{
	if(m_pSprite)
	{
		return m_pSprite->mPath + std::string("/" + std::to_string(index));
	}
	return "";
}

Texture* FrameAnimation::GetFrameTexture(int index)
{ 
	if (!m_pSprite)return nullptr;
	if ( index  < 0  || index >= m_TotalFrameCount) return nullptr;
	
	auto path =  GetFramePath(index);	
	auto* texture = TextureManager::GetInstance()->GetTexture(path);
	if(texture)
	{
		return texture;
	}
	else 
	{
		auto& frame = m_pSprite->mFrames[index];
		return TextureManager::GetInstance()->LoadTexture(path,frame.width,frame.height,true,(uint8_t*)frame.src.data());
	}
}

bool FrameAnimation::IsAttackFrame()
{
	int maxWidthIndex = m_CurrentFrame;
	int maxWidth=0;

	for(int i=0;i<m_GroupFrameCount ;i++)
	{	
		int group =  m_CurrentFrame / m_GroupFrameCount;
		int index =  group*m_GroupFrameCount + i;
		if(index < m_TotalFrameCount)
		{
			auto* texture = GetFrameTexture(index);
			if(texture &&  texture->GetWidth() > maxWidth)
			{
				maxWidth = texture->GetWidth();
				maxWidthIndex = index;
			}
		}
	}
	return m_CurrentFrame == maxWidthIndex;
}

void FrameAnimation::SetCurrentGroup(int group)
{
	if (m_GroupFrameCount == 0)
	{
		m_CurrentGroup = group;
	}
	else 
	{
		m_CurrentFrame = m_CurrentFrame%m_GroupFrameCount + group*m_GroupFrameCount;
		m_CurrentGroup = group;
	}
}


void FrameAnimation::OnUpdate()
{
	if (m_pSprite == nullptr)
	{
		m_pSprite = RESOURCE_MANAGER_INSTANCE->LoadWASSpriteByID(m_ResourceID);
		if (m_pSprite != nullptr)
		{
			m_GroupFrameCount = m_pSprite->mFrameSize;
			m_GroupCount = m_pSprite->mGroupSize;
			m_TotalFrameCount = (int)(m_pSprite->mFrames.size());

			m_KeyX = m_pSprite->mKeyX;
			m_KeyY = m_pSprite->mKeyY;
			m_Width = m_pSprite->mWidth;
			m_Height = m_pSprite->mHeight;
			m_FrameTime = m_FrameTimeBase;
			m_CurrentFrame = m_CurrentGroup * m_GroupFrameCount;
			m_bVisible = true;
			m_DeltaTime = 0;
		}
	}
	else
	{
		if (!m_bVisible)return;
		if (m_CurrentFrame < 0 || m_CurrentFrame >= m_TotalFrameCount) return;

		float dt = WINDOW_INSTANCE->GetDeltaTime();
		if (m_pSprite->mFrames[m_CurrentFrame].IsBlank)
			dt = dt * 2;
		m_DeltaTime += dt;

		m_bIsNextFrameRestart = false;
		m_bCurrentFrameChangedInUpdate = false;
		if (m_DeltaTime >= m_FrameTime)
		{
			m_bCurrentFrameChangedInUpdate = true;
			m_DeltaTime = m_DeltaTime - std::floor(m_DeltaTime / m_FrameTime)*m_FrameTime;

			m_LastFrame = m_CurrentFrame;
			if (!m_pSprite->mFrames[m_LastFrame].IsBlank)
			{
				m_LastNotBlankFrame = m_LastFrame;
			}
			m_CurrentFrame++;
			if (m_GroupFrameCount != 0 && m_CurrentFrame % m_GroupFrameCount == 0)
			{
				if (m_bLoop)
				{
					m_bIsNextFrameRestart = true;
					m_LastFrame = (m_CurrentGroup)* m_GroupFrameCount + m_GroupFrameCount - 1;
					if (m_LastFrame >= m_pSprite->mFrames.size())
					{
						cxlog_err("m_LastFrame over size");
						m_LastFrame = 0;
					}
					if (!m_pSprite->mFrames[m_LastFrame].IsBlank)
					{
						m_LastNotBlankFrame = m_LastFrame;
					}
					m_CurrentFrame = (m_CurrentGroup)* m_GroupFrameCount;
				}
				else
				{
					m_bIsNextFrameRestart = true;
					m_CurrentFrame--;
					m_LastFrame = m_CurrentFrame - 1;
				}
			}
		}
	}
	
}

void FrameAnimation::OnUpdateNew()
{
	if (m_pSprite == nullptr)
	{
		m_pSprite = RESOURCE_MANAGER_INSTANCE->LoadWASSpriteByID(m_ResourceID);
		if (m_pSprite != nullptr)
		{
			m_GroupFrameCount = m_pSprite->mFrameSize;
			m_GroupCount = m_pSprite->mGroupSize;
			m_TotalFrameCount = (int)(m_pSprite->mFrames.size());

			m_KeyX = m_pSprite->mKeyX;
			m_KeyY = m_pSprite->mKeyY;
			m_Width = m_pSprite->mWidth;
			m_Height = m_pSprite->mHeight;
			m_FrameTime = m_FrameTimeBase;
			m_CurrentFrame = m_CurrentGroup * m_GroupFrameCount;
			m_bVisible = true;
		}
	}
	else
	{
		if (m_CurrentFrame < 0 || m_CurrentFrame >= m_TotalFrameCount) return;

		float dt = WINDOW_INSTANCE->GetDeltaTime();

		m_DeltaTime += dt;
		if (m_DeltaTime >= bezier_total_frame_time)
		{
			m_DeltaTime = m_DeltaTime - bezier_total_frame_time;
			if (m_DeltaTime >= 1.0f)
			{
				m_DeltaTime = m_DeltaTime - std::floor(m_DeltaTime);
			}
		}
		int newframe = uniform_bezier_interpolate(bezier_total_frame_time, m_GroupFrameCount, m_DeltaTime);
		m_bIsNextFrameRestart = newframe == m_GroupFrameCount;

		int oldframe = m_CurrentFrame % m_GroupFrameCount;
		m_bCurrentFrameChangedInUpdate = newframe != oldframe;

		if (m_bIsNextFrameRestart)
		{
			if (m_bLoop)
			{
				m_CurrentFrame = m_CurrentGroup * m_GroupFrameCount;
				m_LastFrame = m_CurrentGroup * m_GroupFrameCount + oldframe;
			}
			else
			{
				m_CurrentFrame = m_CurrentGroup * m_GroupFrameCount + oldframe;
				m_LastFrame = m_CurrentFrame - 1;
			}
		}
		else
		{
			m_CurrentFrame = m_CurrentGroup * m_GroupFrameCount + newframe;
			m_LastFrame = m_CurrentFrame - 1;
		}
	}
}

void FrameAnimation::Reset(int group)
{
	SetCurrentGroup(group);
	m_DeltaTime = 0;
	
}

void FrameAnimation::ResetAll()
{
	m_CurrentFrame = 0;
	m_CurrentGroup = 0;
	m_DeltaTime = 0;
}

void FrameAnimation::ResetAnim(int group)
{
	m_CurrentFrame = 0;
	SetCurrentGroup(group);
	m_DeltaTime = 0;	
	m_bIsNextFrameRestart = false;
	// m_CurrentFrame = (m_CurrentGroup)* m_GroupFrameCount;
}

void FrameAnimation::SetVisible(bool visible)
{
	m_bVisible = visible;
}


void FrameAnimation::Draw()
{
	if (m_pSprite == nullptr)return;
	if (!m_bVisible)return;
	if(m_CurrentFrame <  0 || m_CurrentFrame >= m_TotalFrameCount || !m_pSprite) return;
	
	if(m_pSprite->mFrames[m_CurrentFrame].IsBlank)
	{
		m_CurrentFrame = m_LastNotBlankFrame  ;
	}

	auto* texture = GetFrameTexture(m_CurrentFrame);
	if(texture)
	{
		auto& frame = m_pSprite->mFrames[m_CurrentFrame];
		int kx = (m_KeyX - frame.key_x);
		int ky = (m_KeyY - frame.key_y);
		SPRITE_RENDERER_INSTANCE->DrawFrameSprite(texture,
			glm::vec2( m_Pos.x + kx, m_Pos.y + ky),
			glm::vec2(frame.width, frame.height), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
	}
	
}


void FrameAnimation::DrawCenter(float x,float y)
{
	if (m_pSprite == nullptr)return;
	if (!m_bVisible)return;
	if (m_CurrentFrame < 0 || m_CurrentFrame >= m_TotalFrameCount || !m_pSprite) return;

	if (m_pSprite->mFrames[m_CurrentFrame].IsBlank)
	{
		m_CurrentFrame = m_LastNotBlankFrame;
	}

	auto* texture = GetFrameTexture(m_CurrentFrame);
	if (texture)
	{
		auto& frame = m_pSprite->mFrames[m_CurrentFrame];
		int kx = (m_KeyX - frame.key_x);
		int ky = (m_KeyY - frame.key_y);
		SPRITE_RENDERER_INSTANCE->DrawFrameSprite(texture,
			glm::vec2(x - frame.width / 2.f + kx, y - frame.height / 2.f + ky),
			glm::vec2(frame.width, frame.height), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
	}
}


FrameAnimation* lua_check_frame_animation(lua_State*L, int index)
{
	FrameAnimation** ptr = (FrameAnimation**)lua_touserdata(L, index);
	return *ptr;
}
void lua_push_frame_animation(lua_State*L, FrameAnimation* frame_anim)
{
	FrameAnimation** ptr = (FrameAnimation**)lua_newuserdata(L, sizeof(FrameAnimation*));
	*ptr = frame_anim;
	luaL_setmetatable(L, "mt_frame_animation");
}

int frame_animation_method_set_property(lua_State* L)
{
	return 0;
}

int frame_animation_method_get_property(lua_State* L)
{
	return 0;
}

luaL_Reg mt_frame_animation[] = {
	{ "SetProperty",frame_animation_method_set_property },
	{ "GetProperty",frame_animation_method_get_property },
	{ NULL, NULL }
};


void frame_animation_set_bezier_curve_p1_p2(float total_frame_time ,float p1_x, float p1_y, float p2_x, float p2_y)
{
	bezier_total_frame_time = total_frame_time;
	bezier_p1_x = p1_x;
	bezier_p2_y = p1_y;
	bezier_p2_x = p2_x;
	bezier_p2_y = p2_y;
	init_bezier_float_array();
}



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
	 m_pSprite = RESOURCE_MANAGER_INSTANCE->LoadWASSpriteByID(resoureID, true);
	 Width = m_pSprite->mWidth;
	 Height = m_pSprite->mHeight;
	 KeyX = m_pSprite->mKeyX;
	 KeyY = m_pSprite->mKeyY;
	 DirCnt = m_pSprite->mGroupSize;
	 TotalFrames = m_pSprite->mGroupSize*m_pSprite->mFrameSize;
	 GroupFrameCount = m_pSprite->mFrameSize;
	 GroupCount = m_pSprite->mGroupSize;

	 CurrentFrame = 0;
	 for (int i = 0; i < m_pSprite->mFrameSize*m_pSprite->mGroupSize; i++)
	 {
		 UtilsGetFrameTexture(m_pSprite, i);
	 }
	 Pos = { 0,0 };
	 FrameInterval = 0.064f;
	 PlayTime = 0.f;
	 Dir = 0;
	 INPUT_MANAGER_INSTANCE->RegisterView(this);
}

BaseSprite::BaseSprite(uint32_t pkg, uint32_t wasID) :BaseSprite(RESOURCE_MANAGER_INSTANCE->EncodeWAS(pkg, wasID))
{
	
}

BaseSprite::~BaseSprite()
{
	INPUT_MANAGER_INSTANCE->UnRegisterView(this);
}

Bound BaseSprite::GetViewBounds()
{
	return { Pos.x,Pos.x + (float)Width,Pos.y,Pos.y + (float)Height };
}

int BaseSprite::GetViewLayer() const
{
	return 1;
}

bool BaseSprite::CheckDrag(int dx, int dy)
{
	return pow(dx, 2) + pow(dy, 2) >= 16;
}

void BaseSprite::OnDragMove(int dx, int dy)
{
	Pos.x += dx;
	Pos.y += dy;
}

void BaseSprite::Update()
{
	float dt = WINDOW_INSTANCE->GetDeltaTime();
	PlayTime = PlayTime + dt;
	if (PlayTime >= FrameInterval)
	{
		PlayTime = (PlayTime - std::floor(PlayTime / FrameInterval)*FrameInterval) ;
		CurrentFrame = CurrentFrame + 1;
		if (CurrentFrame == GroupFrameCount) {
			CurrentFrame = 0;
		}
	}
}


void BaseSprite::Draw()
{
	auto* texture = UtilsGetFrameTexture(m_pSprite, Dir*GroupFrameCount + CurrentFrame);
	if (texture)
	{
		auto& frame = m_pSprite->mFrames[Dir*GroupFrameCount + CurrentFrame];
		int kx = (m_pSprite->mKeyX- frame.key_x);
		int ky = (m_pSprite->mKeyY- frame.key_y);
		SPRITE_RENDERER_INSTANCE->DrawFrameSprite(texture,
			glm::vec2(Pos.x+ kx, Pos.y +ky),
			glm::vec2(frame.width, frame.height), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
	}
}



BaseSprite* lua_check_base_sprite(lua_State*L, int index)
{
	BaseSprite** ptr = (BaseSprite**)lua_touserdata(L, index);
	return *ptr;
}

int base_sprite_update(lua_State* L){
	auto* base_sprite = lua_check_base_sprite(L, 1);
	base_sprite->Update();
	return 0;
}

int base_sprite_draw(lua_State* L) {
	auto* base_sprite = lua_check_base_sprite(L, 1);
	base_sprite->Draw();
	return 0;
}

int base_sprite_set_pos(lua_State* L){
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
int base_sprite_get_play_time(lua_State* L) {
	auto* base_sprite = lua_check_base_sprite(L, 1);
	lua_pushnumber(L, base_sprite->PlayTime);
	return 1;
}

int base_sprite_get_dir_cnt(lua_State* L) {
	auto* base_sprite = lua_check_base_sprite(L, 1);
	lua_pushinteger(L, base_sprite->DirCnt);
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

int base_sprite_export(lua_State* L) {
	auto* base_sprite = lua_check_base_sprite(L, 1);
	const char* dir = lua_tostring(L, 2);	
	const char* prefix = lua_tostring(L, 3);
	int start = base_sprite->CurrentFrame / base_sprite->GroupFrameCount;
	for(int i=start;i<start+base_sprite->GroupFrameCount;i++){
		std::string filename(dir);
		filename = filename + prefix + std::to_string(i) +".tga";
		base_sprite->m_pSprite->SaveImage(filename.c_str(), i);
	}
	
	return 0;
}




int base_sprite_destroy(lua_State* L){
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
{ "SetFrameInterval",base_sprite_set_frame_interval },
{ "GetFrameInterval",base_sprite_get_frame_interval },
{ "GetWidth", base_sprite_get_width },
{ "GetHeight", base_sprite_get_height },
{ "GetKeyX", base_sprite_get_key_x},
{ "GetKeyY", base_sprite_get_key_y },
{ "GetPlayTime", base_sprite_get_play_time},
{ "GetDirCnt", base_sprite_get_dir_cnt},
{ "GetTotalFrames", base_sprite_get_total_frames},
{ "GetCurrentFrame", base_sprite_get_current_frame},
{ "GetGroupFrameCount", base_sprite_get_group_frame_count},
{ "GetGroupCount", base_sprite_get_group_count },
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
void luaopen_frame_animation(lua_State* L)
{
	if (luaL_newmetatable(L, "mt_frame_animation")) {
		luaL_setfuncs(L, mt_frame_animation, 0);
		lua_setfield(L, -1, "__index");
	}
	else {
		std::cout << "associate mt_frame_animation error!" << std::endl;
	}
	
	init_bezier_float_array();
	script_system_register_function(L, frame_animation_set_bezier_curve_p1_p2);
	script_system_register_luac_function(L, base_sprite_create);
}
