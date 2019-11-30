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

function game_dispatch_message(pt)
	local type = pt:ReadAsInt()
	local js = pt:ReadAllAsString()
	local req = cjson.decode(js)
	cxlog_info('game_dispatch_message', type, js)
	local local_pinfo 
	if  type == PTO_C2C_PLAYER_ENTER then
		local pinfos = req 
		for k,pinfo in ipairs(pinfos) do
			local player = actor_manager_create_actor(pinfo.pid)
			player:SetProperty(PROP_NAME,pinfo.name)
			player:SetProperty(PROP_SCENE_ID,pinfo.scene_id)
			player:SetProperty(PROP_AVATAR_ID,'JXK-KNIFE')
			player:SetProperty(PROP_WEAPON_AVATAR_ID,'JXK-KNIFE-120-晓风残月')
			player:SetPos(pinfo.x, pinfo.y)
			local new_pal = get_pal_from_json('{"1":{"mat":[220,220,512,512,512,510,512,510,512],"to":40,"from":0},"2":{"mat":[211,211,230,445,292,445,306,306,206],"to":80,"from":40},"3":{"mat":[0,0,0,24,24,24,19,19,19],"to":120,"from":80},"4":{"mat":[255,0,0,0,255,0,0,0,255],"to":256,"from":120},"segments":[0,40,80,120,256]}')  
 		   	player:ChangePalMatrix(new_pal)

			if pinfo.is_local then
				local_pinfo = pinfo
			end
		end
		if local_pinfo then
			actor_manager_set_local_player(local_pinfo.pid)
			scene_manager_switch_scene_by_id(local_pinfo.scene_id)
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
    end
end

function input_manager_on_mouse_move(mx, my)
	-- cxlog_info('input_manager_on_mouse_move', mx, my)
end