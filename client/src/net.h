#pragma once

#include "defines.h"
#include "protocol.h"


void net_send_login_message(MSGC2SLogin* msg );
void net_send_logout_message(MSGC2SLogout* msg);
void net_send_move_to_pos_message(std::string name, float x, float y);
void net_send_chat_message(std::string player, std::string msg);
void net_connect_to_server();

void net_manager_connect();
void luaopen_net(lua_State* L);