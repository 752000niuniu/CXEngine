#pragma once

static std::vector<std::string> s_ActionSet= { u8"idle",u8"walk",u8"sit",u8"angry",u8"sayhi",u8"dance",u8"salute",u8"clps",u8"cry",u8"batidle",u8"attack",u8"cast",u8"behit",u8"runto",u8"runback",u8"defend" };;


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
