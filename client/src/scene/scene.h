#pragma once

#include "define_types.h"

#include "event/event.h"
#include "graphics/texture.h"
#include "actor/player.h"
#include "scene/base_scene.h"


/*
场景对象对应一张大地图
场景加载的时候应该加载player ， npc ，场景物件 等等
场景里也包含任务 工会 聊天 战斗等等
为了场景系统跟其他系统接口 ，需要能传递消息
*/
class Scene  : public BaseScene 
{
public:
	Scene(int sceneID,String sceneName);

	virtual ~Scene();

	void ApplyTemplate(const std::map<String, String>& templ);

	virtual void OnLoad() override;
	
	virtual void OnUnLoad() override;

	virtual void Update() override;

	virtual void Draw() override;
	
	int GetMapOffsetX();

	int GetMapOffsetY();

	void SwitchBattle();

	void SetAnnouncement(const char* text);
	
	void SetChat(const char* text);

	FrameAnimation* GetNpcDialogBG();

	void OnSmapClick(float x, float y, float w, float h);
protected:
	
	FrameAnimation* m_NpcDialogBG ;
	FrameAnimation* m_SmapPic;
	ImageView* m_SmapTv;
	uint32_t m_SmapWas;
	bool m_ShowSmap; 
	std::vector<String> m_TransportUUIDs;
	bool m_Switching;
	
};



class BattleScene : public Scene
{
public:
	BattleScene(int sceneID, String sceneName);

	virtual ~BattleScene() override;

	virtual void OnLoad() override;

	virtual void OnUnLoad() override;

	virtual void Update() override;

	virtual void Draw() override;

protected:

	Texture* m_CombatBG;
};


void luaopen_scene(lua_State* L);