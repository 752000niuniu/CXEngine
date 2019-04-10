#pragma once 


#include "core/texture.h"
#include "texture_manager.h"
#include <vector>
#include "sprite_renderer.h"
#include "pos.h"
#include "resource_manager.h"

class FrameAnimation
{
public:

	struct Frame
	{
		int key_x;
		int key_y;
		int width;
		int height;
	};
	
	FrameAnimation(uint64_t resoureID = 0);
	FrameAnimation(uint32_t pkg,uint32_t wasID);

	FrameAnimation& operator=(const FrameAnimation& rhs);
	~FrameAnimation();

	int GetCurrentFrame() { return m_CurrentFrame;}
	bool IsFirstFrame() { return m_CurrentFrame %m_GroupFrameCount ==0 ;}
	int GetLastFrame() { return m_LastFrame;}
	bool IsAttackFrame();
	int GetFrameCount() { return m_TotalFrameCount;}
	int GetCurrentGroup() { return m_CurrentGroup;}
	bool IsCurrentFrameChangedInUpdate() {return m_bCurrentFrameChangedInUpdate;};
	void SetCurrentGroup(int group);
	int GetGroupFrameCount() { return m_GroupFrameCount;}

	int GetKeyX() { return m_KeyX; }
	int GetKeyY() { return m_KeyY; }

	const IntPos GetPos() { return m_Pos; }
	void SetPos(IntPos pos) { m_Pos = pos; }

	void SetVisible(bool visible);

	int GetWidth() { return m_Width; }
	int GetHeight() { return m_Height; }

	void ResetAll();
	void Reset(int group );
	void ResetAnim(int group);
	void ResetFrameTimeByGroupCount(int groupCount);
	
	void SetFrameTimeBase(float base);
	
	void SetPlayLoop(bool loop) {m_bLoop = loop;};
	Texture* GetFrameTexture(int index = -1);
	Sprite* GetSprite() { return  m_pSprite; };
	String GetFramePath(int index); 
	int GetSpritesCount(){ return m_TotalFrameCount;};
	void SetPivotPos(Pos pos)
	{
		m_Pos.x = static_cast<int>(pos.x - m_Width / 2.f);
		m_Pos.y = static_cast<int>(pos.y - m_Height / 2.f);
	}
	void OnUpdate();
	void OnUpdateNew();
	void Draw();
	void DrawCenter(float x, float y);
	bool IsNextFrameRestart(){return m_bIsNextFrameRestart;};

	Bound GetBound() { return Bound{ (float)m_Pos.x, (float)m_Pos.x + (float)m_Width, (float)m_Pos.y,(float)m_Pos.y + (float)m_Height }; }

	void SetWidth(int w){ m_Width = w;}
	void SetHeight(int h){m_Height= h;}

	uint64_t GetResourceID() { return m_ResourceID; };

	bool IsVisible() { return m_bVisible; }

private:
	int m_TotalFrameCount;  //总帧数
	int m_CurrentFrame;		//当前帧
	int m_LastFrame;		//上一帧
	int m_CurrentGroup;		//当前方向
	int m_GroupFrameCount;	//组帧数
	int m_GroupCount;		//组数
	int m_KeyX;			
	int m_KeyY;
	IntPos m_Pos;		
	int m_Width;
	int m_Height;	
	bool m_bLoop;			//循环播放
	bool m_bIsNextFrameRestart;
	float m_DeltaTime;
	bool m_bVisible;		
	bool m_bCurrentFrameChangedInUpdate;
	int m_LastNotBlankFrame;
	float m_FrameTimeBase;
	float m_FrameTime;
	Sprite* m_pSprite;
	uint64_t m_ResourceID;
};


class BaseSprite
{

public:
	struct Frame
	{
		int KeyX;
		int KeyY;
		int Width;
		int Height;
	};
	using FrameVector = std::vector<Frame>;

	BaseSprite(uint64_t resoureID = 0);
	BaseSprite(uint32_t pkg, uint32_t wasID);
	~BaseSprite();
	
	void Update();
	
	void Draw();

	int DirCnt;
	int TotalFrames;
	int CurrentFrame;
	int GroupFrameCount;
	int GroupCount;
	
	int Width;
	int Height;
	int KeyX;
	int KeyY;

	int Dir;
	Pos Pos;
	FrameVector Frames;
	Sprite* m_pSprite;
	float FrameInterval;
	float PlayTime;
};

void lua_push_frame_animation(lua_State*L, FrameAnimation* frame_anim);
void luaopen_frame_animation(lua_State* L);