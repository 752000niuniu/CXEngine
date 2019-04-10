#pragma once

#include "message.h"

class BaseGameEntity
{
public:

	BaseGameEntity(const std::string& name) : m_Name(name), m_ID(NextEntityID()) {}

	BaseGameEntity():BaseGameEntity(""){}

	virtual ~BaseGameEntity() {} 

	const std::string & GetName() const { return m_Name; }

	void SetName(const std::string& name) { m_Name = name; }

	uint64_t GetID() { return m_ID; }

    virtual bool HandleMessage(const Telegram& msg){return false;};// = 0;

    virtual void Update(){};// = 0;

protected:
	std::string m_Name;
private:

	uint64_t m_ID;

private:
	static uint64_t NextEntityID()
	{
		static uint64_t id = 0x10ABCDEF;
		return id++;
	}
};

class GameEntityManager final: public Singleton<GameEntityManager>
{
public:
	friend class Singleton<GameEntityManager>;
private:	
	typedef std::map<uint64_t, BaseGameEntity*> EntityMap;
private:
	EntityMap m_EntityMap;
	GameEntityManager() {} 
	~GameEntityManager() {} 
public:
	void RegisterEntity(BaseGameEntity* pEntity)
	{
		m_EntityMap[pEntity->GetID()] = pEntity;
	};
	
	BaseGameEntity* GetEntityByID(uint64_t id) 
	{
		if(m_EntityMap.find(id) != m_EntityMap.end())
		{
			return m_EntityMap[id];
		}
		else return nullptr;
	};

	void RemoveEntity(BaseGameEntity* pEntity)
	{
		m_EntityMap.erase(pEntity->GetID());
	}
};

#define GAME_ENTITY_MANAGER_INSTANCE GameEntityManager::GetInstance()
