
#pragma once
#include "ui.h"
#include <NESupport.h>

class BaseSprite : public View
{
public:
	BaseSprite(uint64_t resoureID = 0);
	BaseSprite(uint32_t pkg, uint32_t wasID);
	virtual ~BaseSprite();

	Bound GetViewBounds()  override;
	int GetViewLayer()const override;
	bool CheckDrag(int dx, int dy) override;
	void OnDragMove(int dx, int dy) override;

	virtual void Update();
	void Draw();
	void SetLoop(bool loop);
	void Reset();
	void Stop();
	void Play();

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
	int bLoop;
	int bPlay;
	int bGroupEndUpdate;
	Pos Pos;
	Sprite* m_pSprite;
	float FrameInterval;
	float PlayTime;
};


void luaopen_sprite(lua_State* L);