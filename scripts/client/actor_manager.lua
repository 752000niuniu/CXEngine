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
	if not actor:IsLocal() then
		local player = actor_manager_fetch_local_player()
		if player then
			player:SetTarget(actor)
		end
		if player:GetProperty(PROP_IS_COMBAT) then
			combat_system_actor_ev_on_click(actor, button, x, y)
		else
			npc_dialog_show(true,'神州上下祸劫频生，灵石是否重补苍天裂痕，', {
				{ 
					txt = '我是找你打架的',
					func = function()
						local msg = {}
						msg.atk = player:GetID()
						msg.def = actor:GetID()
						net_send_message(PTO_C2S_COMBAT_START, cjson.encode(msg))
	
						
						local player = actor_manager_fetch_local_player()
						player:StopMove()
					end
				},
				{ 
					txt ='相信你是冤枉的',
					func=function()
						cxlog_info('相信你是冤枉的')
					end
				},
				{ 
					txt='告辞',
					func=function()
						cxlog_info('告辞')
					end
				}
			})
		end
		
	end
end

