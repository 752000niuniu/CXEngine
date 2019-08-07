#pragma once

#include <string>

std::string action_get_name(int i);
size_t action_get_size();
std::string action_system_get_action(int i);
class Action
{
public:
	enum State
	{
		Idle = 0,
		Walk,
		Sit,
		Angry,
		Sayhi,
		Dance,
		Salute,
		Clps,
		Cry,
		Batidle,
		Attack,
		Cast,
		Behit,
		Runto,
		Runback,
		Defend
	};

	Action();
	~Action();

private:

};

class Direction
{
public:
	enum Dir
	{
		N = 6,
		S = 4,
		W = 5,
		E = 7,
		N_E = 3,
		N_W = 2,
		S_E = 0,
		S_W = 1,
	};
};
