#pragma once

#include "game_map.h"
#include "../actor/Player.h"
#include "base_scene.h"

using String = std::string;
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

	
	int GetMapOffsetX();

	int GetMapOffsetY();

	void SwitchBattle();

	void SetAnnouncement(const char* text);
	
	void SetChat(const char* text);


	void OnSmapClick(float x, float y, float w, float h);
protected:
	void ProcessInput();
	
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


protected:

};


