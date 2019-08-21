
#pragma once
#include "ui.h"

struct NE::Sprite;
struct NE::Sprite::Sequence;
class BaseSprite : public View
{
public:
	
	BaseSprite(uint64_t resoureID = 0);
	BaseSprite(uint32_t pkg, uint32_t wasID);
	virtual ~BaseSprite();

	virtual void Update();
	virtual void Draw();
	void SetLoop(bool loop);
	void Reset();
	void Stop();
	void Play();
	NE::Sprite::Sequence* GetFrame(int index = -1);
	int GetFrameKeyX(int index = -1);
	int GetFrameKeyY(int index = -1);
	int GetFrameWidth(int index = -1);
	int GetFrameHeight(int index = -1);

	void EnableDrag(bool enable);

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
	int bFrameUpdated;
	Pos Pos;
	NE::Sprite* m_pSprite;
	float FrameInterval;
	float PlayTime;
	bool Visible;
	bool bEnableDrag;
	int AttackKeyFrame;
	Bound GetViewBounds() override;
	bool CheckDrag(int x, int y) override;
	void OnDragMove(int x, int y)override;
};


class Animation : public BaseSprite
{
public:
	Animation(uint64_t resoureID = 0);
	Animation(uint32_t pkg, uint32_t wasID) :BaseSprite(pkg, wasID) {  };
	virtual ~Animation() {};
	void Update() override;
	void Draw() override;
	
};

void lua_push_base_sprite(lua_State*L, BaseSprite* sprite);
void luaopen_sprite(lua_State* L);