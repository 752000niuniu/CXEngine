
#pragma once
#include "ui.h"
#include <functional>

struct NE::Sprite;
struct NE::Sprite::Sequence;
using CXPos = Pos;
class BaseSprite : public View
{
public:
	BaseSprite(uint64_t resoureID = 0);
	BaseSprite(uint32_t pkg, uint32_t wasID);
	virtual ~BaseSprite();

	virtual void Update() {};
	virtual void Draw() {};
	
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
	uint64_t ResID;
	CXPos Pos;
	NE::Sprite* m_pSprite;
	float FrameInterval;
	float PlayTime;
	
	bool bEnableDrag;
	Bound GetViewBounds() override;
	bool CheckDrag(int x, int y) override;
	void OnDragMove(int x, int y)override;

};

enum EAnimationState
{
	ANIMATION_PLAY = 0,
	ANIMATION_STOP,
	ANIMATION_PAUSE
};

class Animation : public BaseSprite
{
public:
	Animation(uint64_t resoureID = 0);
	Animation(uint32_t pkg, uint32_t wasID);
	virtual ~Animation() {};
	void Update() override;
	void Draw() override;
	void SetVisible(bool visible) { m_Visible = visible; };
	bool GetVisible() { return m_Visible; };

	void Pause(int ms);
	void SetLoop(int loop);
	void Reset();
	void Stop();
	void Play();
	void Replay();
	
	void AddFrameCallback(int frame, std::function<void()> callback);
	void SetFrameInterval(float interval) { FrameInterval = interval; };
	float GetFrameInterval() { return FrameInterval; };

	void SetDir(int dir) { Dir = dir; };
	int GetDir() { return Dir; };
	bool IsGroupEndUpdate() { return m_bGroupEndUpdate; };
	bool IsFrameUpdate() { return m_bFrameUpdate; };
	int GetAttackKeyFrame() { return AttackKeyFrame; };
	
	void Translate(CXPos pos, int duration);
	void LockFrame(int frame);
	void UnLockFrame();
	int GetState() { return m_State; };
private:
	bool m_bGroupEndUpdate;
	bool m_bFrameUpdate;
	int m_LoopCount;
	int AttackKeyFrame;
	int bPlay;
	int m_PreviousState;
	int m_State;
	bool m_Visible;
	int m_PauseTime;
	std::map<int, std::function<void()>> m_Callbacks;
	int m_LockFrame;
	bool m_bLockFrame;
	bool m_bTranslate;
	CXPos m_TranslatePos;
	float m_Duration;
	CXPos m_Velocity;
	bool m_bLoop;
};

class BeatNumber 
{
public:
	struct Digit
	{
		int8_t digit;
		float y;
	};
	BeatNumber();
	virtual ~BeatNumber() { }
	void Update();
	void Draw();
	void SetPos(float x, float y);
	void SetNumber(int num);
	void Beat();
	void SetVisible(bool visible) { m_Visible = visible; };
	bool GetVisible() { return m_Visible; };
private:
	bool m_bBeat;
	bool m_Visible;
	float m_PlayTime;
	float m_BeatTime;
	float m_PauseTime;
	float m_BeatHeights;
	float m_AdvanceX;
	int m_Number;
	CXPos m_Pos;
	std::deque<Digit>m_Digits;
	Animation m_HealAnim;
	Animation m_HitAnim;
};

class AnimationManager :public Singleton<AnimationManager>
{
public:
	AnimationManager() ;
	~AnimationManager() ;
	void AddQueue(Animation* animation);
	void Update();
	void Draw();
private:
	std::vector<Animation*> m_Animations;
	
};

void lua_push_animation(lua_State*L, Animation* sprite);
void luaopen_sprite(lua_State* L);