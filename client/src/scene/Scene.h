#pragma once

#include "define_types.h"

#include "../i_update_draw.h"
#include "../Event/Event.h"
#include "core/Texture.h"
#include "game_map.h"
#include "../actor/Player.h"
#include "base_scene.h"


/*
���������Ӧһ�Ŵ��ͼ
�������ص�ʱ��Ӧ�ü���player �� npc ��������� �ȵ�
������Ҳ�������� ���� ���� ս���ȵ�
Ϊ�˳���ϵͳ������ϵͳ�ӿ� ����Ҫ�ܴ�����Ϣ
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
	void ProcessInput();
	
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