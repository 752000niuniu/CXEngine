
#pragma once
#include "ui.h"

struct NE::Sprite;
class BaseSprite
{
public:
	BaseSprite(uint64_t resoureID = 0);
	BaseSprite(uint32_t pkg, uint32_t wasID);
	virtual ~BaseSprite();

	virtual void Update();
	void Draw();
	void SetLoop(bool loop);
	void Reset();
	void Stop();
	void Play();


	int TotalFrames;
	int CurrentFrame;
	int GroupFrameCount;
	int GroupCount;
	int Width;
	int Height;
	int KeyX;
	int KeyY;
	int Dir;
	int bLoop;
	int bPlay;
	int bGroupEndUpdate;
	Pos Pos;
	NE::Sprite* m_pSprite;
	float FrameInterval;
	float PlayTime;
};

void lua_push_base_sprite(lua_State*L, BaseSprite* sprite);
void luaopen_sprite(lua_State* L);