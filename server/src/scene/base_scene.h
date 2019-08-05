#pragma once

#include "define_types.h"
#include "entity.h"
#include "game_map.h"
#include "actor/player.h"

/*
��������Ļ���
���Բ�ֳ�SplashScene , UIScene , Scene����Ϸ������
*/
class BaseScene
{
public:
	BaseScene(int sceneID, String sceneName);

	virtual ~BaseScene();

	virtual void Update() = 0;

	virtual void Draw() {};

	void AddPlayer(const char* player_name, float x, float y, int dir, int role_id, int action_id);

	void AddNpc(const char* player_name, float x, float y, int dir, int role_id, int action_id, const char* msg);

	void AddPet(const char* player_name, int x, int y, int dir, int role_id, int action_id);

	void AddPlayer(Player* player);

	void ClearAllPlayers();

	void SetPlayerByName(const char* player_name);

	void SetPlayerByIndex(int index);

	void SetSceneID(int id) { m_SceneID = id; };

	int GetSceneID() { return m_SceneID; };

	bool IsLoading() { return m_IsLoading; }

	void SetLoading(bool load) { m_IsLoading = load; }

	Pos GetBirthPos() { return  m_BirthPos; };

	void SetBirthPos(Pos pos) { m_BirthPos = pos; };

	void SetLoadingCenter(Pos pos) { m_LoadingCenter = pos; }

	int GetOffsetX() { return m_OffsetX; };

	int GetOffsetY() { return m_OffsetY; }

	void SetName(std::string str) { m_Name = str; }

	std::string GetName() { return m_Name; }

	Player* GetLocalPlayer() { return m_LocalPlayer; };

	Player* GetPlayerByNickname(const char* nickname);

	Player* GetPlayerByIndex(int index) { if (index < 0 || index >= m_Players.size())return nullptr; else return m_Players[index]; }

	GameMap* GetGameMap() { return m_Map; };

	size_t GetPlayersCount() { return m_Players.size(); }

	void Load();

	void UnLoad();

protected:
	virtual void OnLoad() = 0;

	virtual void OnUnLoad() = 0;

	std::string m_Name;

	int m_SceneID;

	int m_Width;

	int m_Height;

	int m_OffsetX;

	int m_OffsetY;

	bool m_IsLoading;

	bool m_Loaded;

	Pos m_BirthPos;

	Pos m_LoadingCenter;

	GameMap* m_Map;

	Player* m_LocalPlayer;

	std::vector<Player*> m_Players;

};


