#pragma once


#include <cxrandom.h>
#include "define_types.h"
#include "animation/frame_animation.h"


class SkillManager : public Singleton<SkillManager>
{
	friend class Singleton<SkillManager>;
public:
	FrameAnimation* GetSkillByID(int id){ 
		assert(m_Skills.find(id) != m_Skills.end());
		return m_Skills[id]; 
	};
	uint32_t GetRandomSkill(){
		/*int i=0;
		for(auto& it : m_Skills)
		{
			if(i++ == m_RandomIndex) return &(it.second);
		}*/

		return m_SkillIDs[m_RandomIndex];
	};

	void SetRandomSkillID(){ 

		m_RandomIndex=RANDOM_INSTANCE->NextInt(0,static_cast<int>( m_SkillIDs.size()-1));
	};

protected:
	SkillManager();
private:
	int m_RandomID;
	int m_RandomIndex;
	std::map<int,int> m_SkillsIDMap;
	std::map<int,FrameAnimation*> m_Skills;
	std::vector<uint32_t> m_SkillIDs;
};


#define SKILL_MANAGER_INSTANCE SkillManager::GetInstance()


class Skill
{
public:
	Skill();
	~Skill();
	
private:

};

class Buff
{
public:
	Buff();
	~Buff();

private:

};

