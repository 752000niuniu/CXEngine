local tActors = {}
local ACTOR_ID_COUNTER = 1

local actor_table = {}

local function utils_find_role_id_row(actor_type, role_id)
    local role_tbl =  content_system_get_table('role')
    local pet_tbl =  content_system_get_table('pet')
    -- local weapon_tbl =  content_system_get_table('weapon')
    local npc_tbl =  content_system_get_table('npc')

    local row_pos =  0
    if actor_type == ACTOR_TYPE_PLAYER then
        row_pos = role_tbl[role_id].row_pos
    elseif actor_type == ACTOR_TYPE_SUMMON then
        row_pos = pet_tbl[role_id].row_pos
    elseif actor_type == ACTOR_TYPE_NPC then
        row_pos = npc_tbl[role_id].row_pos
    end
    print(actor_type, role_id , row_pos )
    return row_pos-1
end

function actor_manager_init()
    
end

function actor_manager_deinit()
    
end

function actor_manager_add_new(tid)

end

function actor_ev_on_click(actor, button, x, y)
	cxlog_info('ACTOR_EV_ON_CLICK',button,x,y)
	local player = actor_manager_fetch_local_player()
    local msg = {}
    msg.pid = player:GetID()
    msg.target = actor:GetID()
    net_send_message(PTO_C2S_CLICK_NPC, cjson.encode(msg) )
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

stub[PTO_S2C_CREATE_ACTOR] = function(req)
    local pid = req[tostring(PROP_ID)]
    local actor = actor_manager_create_actor(pid)
    actor:SetProperties(req)
    actor:ResetASM()

end