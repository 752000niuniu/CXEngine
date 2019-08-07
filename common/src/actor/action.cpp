#include "action.h"


Action::Action()
{
}

Action::~Action()
{
}

static std::vector<std::string> s_ActionSet = { u8"idle",u8"walk",u8"sit",u8"angry",u8"sayhi",u8"dance",u8"salute",u8"clps",u8"cry",u8"batidle",u8"attack",u8"cast",u8"behit",u8"runto",u8"runback",u8"defend" };;


std::string action_get_name(int i)
{
	return s_ActionSet[i];
}

size_t action_get_size()
{
	return s_ActionSet.size();
}

std::string action_system_get_action(int i)
{
	return s_ActionSet[i];
}
