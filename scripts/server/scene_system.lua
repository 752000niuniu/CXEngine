
local npcs = { }

function scene_system_init()
    local ostime = os.time()
    local npc_table = content_system_get_table('npc')
    for i, props in ipairs(npc_table) do
        local npc  = actor_manager_create_actor(ostime)
        npc:SetProperties(props)    
        npc:SetProperty(PROP_HP, npc:GetMaxHP())
        npc:SetProperty(PROP_MP, npc:GetMaxMP())
        ostime = ostime + 1
    end
end

function scene_system_update()

end

function scene_system_stop()
end


stub[PTO_C2S_CLICK_NPC] = function(req)
    local player = actor_manager_fetch_player_by_id(req.pid)
    local target = actor_manager_fetch_player_by_id(req.target)
    if player and target then
        player:SetTarget(target)

        -- local player = actor_manager_fetch_local_player()
		-- if player then
		-- 	player:SetTarget(actor)
		-- end
		-- if player:GetProperty(PROP_IS_COMBAT) then
		-- 	combat_system_actor_ev_on_click(actor, button, x, y)
		-- else
		-- 	npc_dialog_show(true,'神州上下祸劫频生，灵石是否重补苍天裂痕，', {
		-- 		{ 
		-- 			txt = '我是找你打架的',
		-- 			func = function()
		-- 				local msg = {}
		-- 				msg.atk = player:GetID()
		-- 				msg.def = actor:GetID()
		-- 				net_send_message(PTO_C2S_COMBAT_START, cjson.encode(msg))
	
						
		-- 				local player = actor_manager_fetch_local_player()
		-- 				player:StopMove()
		-- 			end
		-- 		},
		-- 		{ 
		-- 			txt ='相信你是冤枉的',
		-- 			func=function()
		-- 				cxlog_info('相信你是冤枉的')
		-- 			end
		-- 		},
		-- 		{ 
		-- 			txt='告辞',
		-- 			func=function()
		-- 				cxlog_info('告辞')
		-- 			end
		-- 		}
		-- 	})
		-- end
        net_send_message(req.pid, PTO_S2C_CLICK_NPC)
    end
end 