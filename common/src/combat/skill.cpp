#include "skill.h"

#ifndef SIMPLE_SERVER
#include "file_system.h"
#include <tsv.h>
#include "animation/frame_animation.h"

SkillManager::SkillManager()
{
	m_RandomID=-1;
	m_RandomIndex = 0;
	utils::tsv skillTSV(FileSystem::GetTSVPath("magic"));
	m_Skills.clear();

	for (auto& row : skillTSV.Rows)
	{
		auto _was_id = row["was_id"];
		uint32 was_id = std::stoul(_was_id, 0, 16);
		m_SkillIDs.push_back(was_id);
		m_Skills.insert({ was_id, new FrameAnimation(MAGICWDF, was_id) });
	}
}



Skill::Skill()
{
}

Skill::~Skill()
{
}


Buff::Buff()
{
}

Buff::~Buff()
{
}
#endif