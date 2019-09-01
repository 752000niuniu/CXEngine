
SCREEN_WIDTH = 1200
SCREEN_HEIGHT = 800
DefaultMapID = 1001

script_system_dofile('../share/vfs.lua')
script_system_dofile('../share/utils.lua')

script_system_dofile 'content_system.lua'
script_system_dofile 'actor_metatable.lua'
script_system_dofile 'scene_manager.lua'
script_system_dofile 'actor_manager.lua'
-- script_system_dofile('generator/imgui_binding_generator.lua')
-- script_system_dofile('generator/table_template.lua')
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
	print('game_dispatch_message', type, js)
	local local_pinfo 
	if  type == PTO_C2C_PLAYER_ENTER then
		local pinfos = req 
		for k,pinfo in ipairs(pinfos) do
			local player = actor_manager_create_actor(pinfo.pid)
			player:SetName(pinfo.name)
			player:SetSceneID(pinfo.scene_id)
			player:SetAvatarID('JXK-KNIFE')
			player:SetWeaponAvatarID('JXK-KNIFE-120-晓风残月')
	
			-- player:SetAvatarID('JXK-KNIFE')
   			-- player:SetWeaponAvatarID('JXK-KNIFE-120-晓风残月')

			player:SetX(pinfo.x)
			player:SetY(pinfo.y)
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