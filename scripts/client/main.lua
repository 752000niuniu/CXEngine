SCREEN_WIDTH = 1200
SCREEN_HEIGHT = 800
DefaultMapID = 1001

script_system_dofile('../share/enums.lua')
script_system_dofile('../share/vfs.lua')
script_system_dofile('../share/utils.lua')
script_system_dofile('../share/content_system.lua')
script_system_dofile('../share/actor_metatable.lua')

script_system_dofile 'actor_metatable.lua'
script_system_dofile('../combat/combat_system.lua')
script_system_dofile 'scene_manager.lua'
script_system_dofile 'actor_manager.lua'
script_system_dofile('action/action.lua')
script_system_dofile('ui_renderer.lua')
script_system_dofile('event_system.lua')
script_system_dofile('input_manager.lua')

-- script_system_dofile('generator/imgui_binding_generator.lua')
-- script_system_dofile('generator/table_template.lua')
-- script_system_dofile('../generator/actor_template.lua')
-- script_system_dofile('parser.lua')

-- luadbg_listen(9527)
IsCombat = false
--聊天背景框 wzife 39D3BD99 
function main()
    window_system_init(SCREEN_WIDTH,SCREEN_HEIGHT); window_system_show()	
end

function on_script_system_init()
    -- generate_avatar_role_tsv()
    content_system_init()
    net_manager_init('127.0.0.1', 45000)
    text_renderer_init()
    timer_manager_init()
    input_manager_init()
    resource_manager_init()
    sprite_renderer_init()
    imgui_init()
    scene_manager_init()
	actor_manager_init()
	combat_system_init()
	asm_system_init()


end

function on_script_system_update()
    net_manager_update()
    timer_manager_update()
    resource_manager_update()
    scene_manager_update()
    return true
end

function on_script_system_draw()
	scene_manager_draw()
end

function on_script_system_deinit()
    net_manager_deinit()
    timer_manager_deinit()
    input_manager_deinit()
    resource_manager_deinit()
    scene_manager_deinit()
    actor_manager_deinit()
end

function on_player_send_chat_message(msg)
	local player = actor_manager_fetch_local_player()
	player:Say(msg)
	clear_chat_text_cache()

	local req = {}
	req.pid = player:GetID()
	req.msg = msg
	net_send_message(PTO_C2C_CHAT, cjson.encode(req))
end
function actor_ev_on_click(actor, button, x, y)
	cxlog_info('ACTOR_EV_ON_CLICK',button,x,y)
	if not actor:IsLocal() then
		local player = actor_manager_fetch_local_player()
		if player then
			player:SetTarget(actor)
		end
		npc_dialog_show(true,'神州上下祸劫频生，灵石是否重补苍天裂痕，', {
			{ 
				txt = '我是找你打架的',
				func = function()
					local msg = {}
					msg.atk = player:GetID()
					msg.def = actor:GetID()
					net_send_message(PTO_C2S_COMBAT_START, cjson.encode(msg))

					
					local player = actor_manager_fetch_local_player()
					local target = player:GetTarget()
					player:SetProperty(PROP_HP, player:GetMaxHP()/3)
					target:SetProperty(PROP_HP, target:GetMaxHP()/3)
					player:StopMove()
	
					-- combat_system_start_battle({player},{target})
	
					cxlog_info(player:GetProperty(PROP_NAME), target:GetProperty(PROP_NAME))
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

function game_dispatch_message(pt)
	local type = pt:ReadAsInt()
	local js = pt:ReadAllAsString()
	local req = cjson.decode(js)
	cxlog_info('game_dispatch_message', type, js)
	
	if  type == PTO_C2C_PLAYER_ENTER then
		for i,actor_info in ipairs(req.actors) do
			local actor = actor_manager_create_actor(actor_info[tostring(PROP_ID)])
			actor:SetProperties(actor_info)

			actor_reg_event(actor, ACTOR_EV_ON_CLICK, actor_ev_on_click)
			actor_reg_event(actor, ACTOR_EV_ON_HOVER, function(actor, x, y)
				
			end)
		end
		if req.local_pid then
			actor_manager_set_local_player(req.local_pid)
			local player = actor_manager_fetch_player_by_id(req.local_pid)
			scene_manager_switch_scene_by_id(player:GetProperty(PROP_SCENE_ID))	
		end
 
	elseif type == PTO_C2C_CHAT then
		local player = actor_manager_fetch_player_by_id(req.pid)
		if not player:IsLocal() then
			player:Say(req.msg)
		end
	elseif type == PTO_C2C_MOVE_TO_POS then
		local player = actor_manager_fetch_player_by_id(req.pid)
		if not player:IsLocal() then
			player:MoveTo(req.x,req.y)
		end
	elseif type == PTO_S2C_SYNC_PROPS then
		-- for i, dirty_prop in ipairs(req) do
		-- 	local pid = dirty_prop[1]
		-- 	local p = actor_manager_fetch_player_by_id(pid)
		-- 	if p then
		-- 		p:SetProperty(dirty_prop[2] ,dirty_prop[3])
		-- 		cxlog_info(' p ',p, ' propid ',dirty_prop[2] ,dirty_prop[3])
		-- 	end
		-- end
	elseif type == PTO_S2C_COMBAT_START then
		-- combat_system_start_battle()
		local atk = actor_manager_fetch_player_by_id(req.atk)
		local def = actor_manager_fetch_player_by_id(req.def)
		combat_system_start_battle({atk},{def})
    end	
end

function input_manager_on_mouse_move(mx, my)
	-- cxlog_info('input_manager_on_mouse_move', mx, my)
end