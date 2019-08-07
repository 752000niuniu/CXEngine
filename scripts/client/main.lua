luadbg_listen(9528)

SCREEN_WIDTH = 1200
SCREEN_HEIGHT = 800
DefaultMapID = 1001

script_system_dofile('../share/vfs.lua')
script_system_dofile('../share/utils.lua')

script_system_dofile 'content_system.lua'
script_system_dofile 'actor_metatable.lua'
script_system_dofile 'scene_manager.lua'
script_system_dofile 'actor_manager.lua'
script_system_dofile 'table_template.lua'

-- script_system_dofile 'imgui_binding_generator.lua'

IsCombat = false

--聊天背景框 wzife 39D3BD99 
function main()
    window_system_init(SCREEN_WIDTH,SCREEN_HEIGHT)
    window_system_show()	
end

function on_script_system_init()
    -- generate_avatar_role_tsv()
    content_system_init()
    net_manager_init()
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
    actor_manager_update()
    return true
end

function on_script_system_draw()
    scene_manager_draw()
end

function on_script_system_deinit()
    luadbg_stop()
    net_manager_deinit()
    timer_manager_deinit()
    input_manager_deinit()
    resource_manager_deinit()
    scene_manager_deinit()
    actor_manager_deinit()
end



		-- break;
		-- case PTO_S2C_CHAT:
		-- {
		-- 	MSGC2SChat chatmsg;
		-- 	chatmsg.namelen = pt->ReadAsInt32();
		-- 	chatmsg.name = pt->ReadAsString(chatmsg.namelen);
		-- 	chatmsg.ctlen = pt->ReadAsInt32();
		-- 	chatmsg.content = pt->ReadAsString(chatmsg.ctlen);
		-- 	Player* player = scene_find_player(chatmsg.name.c_str());
		-- 	if (player) player->Say(chatmsg.content);
		-- }
		-- break;
		-- case PTO_S2C_MOVE_TO_POS:
		-- {
		-- 	int namelen = pt->ReadAsInt32();
		-- 	std::string name = pt->ReadAsString(namelen);
		-- 	float pos_x = pt->ReadAsFloat();
		-- 	float pos_y = pt->ReadAsFloat();
		-- 	Player* player = scene_find_player(name.c_str());
		-- 	if (player) player->MoveTo(pos_x, pos_y);
		-- }
		-- break;
		-- default:
		-- 	break;
		-- }
        -- delete pt;
        
function game_dispatch_message(pt)
    print('game_dispatch_message' , pt:Preview(pt:readable_size()))
    local type = pt:ReadAsInt()
    if  type == PTO_S2C_PLAYER_ENTER then
		local msg = cjson.decode(pt:ReadAllAsString())
		local player = actor_manager_create_player(msg.pid)
		player:SetName(msg.name)
		player:SetSceneID(msg.scene_id)
		player:SetRoleID(msg.role_id)
		player:SetWeaponID(40)
		player:SetX(msg.x)
		player:SetY(msg.y)

		actor_manager_set_local_player(msg.pid)
		scene_manager_switch_scene_by_id(msg.scene_id)
	elseif type == PTO_S2C_CHAT then
		
	elseif type == PTO_S2C_MOVE_TO_POS then
		local msg = cjson.decode(pt:ReadAllAsString())
		local player = actor_manager_fetch_player_by_id(msg.pid)
		player:MoveTo(msg.x,msg.y)
    end
end