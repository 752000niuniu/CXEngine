
stub = stub or {}


function on_player_send_chat_message(msg)
	local player = actor_manager_fetch_local_player()
	player:Say(msg)
	clear_chat_text_cache()

	local req = {}
	req.pid = player:GetID()
	req.msg = msg
	net_send_message(PTO_C2C_CHAT, cjson.encode(req))
end


stub[PTO_C2C_PLAYER_ENTER] = function(req)
	for i,actor_info in ipairs(req.actors) do
		local actor = actor_manager_create_actor(actor_info[tostring(PROP_ID)])
		actor:SetProperties(actor_info)

		-- actor_reg_event(actor, ACTOR_EV_ON_CLICK, actor_ev_on_click)
	end
	if req.local_pid then
		actor_manager_set_local_player(req.local_pid)
	end
end

stub[PTO_C2C_NPC_ENTER] = function(req)
	for i,actor_info in ipairs(req.npcs) do
		local actor = actor_manager_create_actor(actor_info[tostring(PROP_ID)])
		actor:SetProperties(actor_info)
	end
end


stub[PTO_C2C_CHAT] = function(req)
	local player = actor_manager_fetch_player_by_id(req.pid)
	if not player:IsLocal() then
		player:Say(req.msg)
	end
end

stub[PTO_C2C_MOVE_TO_POS] = function(req)
	local player = actor_manager_fetch_player_by_id(req.pid)
	if not player:IsLocal() then
		player:MoveTo(req.x,req.y)
	end
end

stub[PTO_S2C_SYNC_PROPS] = function(req)
	for i, dirty_prop in ipairs(req) do
		local pid = dirty_prop[1]
		local p = actor_manager_fetch_player_by_id(pid)
		if p then
			p:SetProperty(dirty_prop[2] ,dirty_prop[3])
			cxlog_info(' p ',p, ' propid ', prop_id_to_name( dirty_prop[2]) ,dirty_prop[3])
		end
	end
end

stub[PTO_S2C_CLICK_NPC] = function(req)
	local player = actor_manager_fetch_player_by_id(req.pid)
    local target = actor_manager_fetch_player_by_id(req.target)
    if player and target then
		player:SetTarget(target)
		if player:GetProperty(PROP_IS_COMBAT) then
			combat_system_actor_ev_on_click(target)
		else
			npc_on_show_dialog(player, target)
		end
	end
end

function game_dispatch_message(pt)
	local type = pt:ReadAsInt()
	local js = pt:ReadAllAsString()
	local req = cjson.decode(js)
	cxlog_info('game_dispatch_message', type, js)
	if stub[type] then
		stub[type](req)
	end
end

USER_ACCOUNT = command_arg_opt_str('user', 'oceancx')
USER_PASSWORD = command_arg_opt_str('pass', '123456')
function game_server_on_connection(connected)
	if connected then
		local msg = {}
		msg.account = USER_ACCOUNT
		msg.password = USER_PASSWORD
		msg.name = 'Ocean藏心'
		msg.scene_id = -105
		msg.role_id = 1
		msg.weapon_id = 40
		msg.x = 306
		msg.y = 466
		net_send_message(PTO_C2C_LOGIN, cjson.encode(msg))
	else
		os.execute('exit()')
	end
end

function game_server_on_disconnect()

end



function net_manager_stub()
	return stub
end


function net_manager_player_dostring(code)
	local player = actor_manager_fetch_local_player()
	if player then
		local req = {
			pid = player:GetID(),
			code = code
		}
		net_send_message(PTO_C2S_PLAYER_DOSTRING,cjson.encode(req))
	end
end

