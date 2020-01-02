SCREEN_WIDTH = 1200
SCREEN_HEIGHT = 800
DefaultMapID = 1001


script_system_dofile('../share/enums.lua')
script_system_dofile('../share/enums_protocol.lua')
script_system_dofile('../share/vfs.lua')
script_system_dofile('../share/utils.lua')
script_system_dofile('../share/content_system.lua')
script_system_dofile('../share/actor_metatable.lua')

script_system_dofile 'actor_metatable.lua'

script_system_dofile('net_manager.lua')

-- script_system_dofile('../combat/combat_system.lua')
script_system_dofile 'scene_manager.lua'
script_system_dofile 'actor_manager.lua'
script_system_dofile('action/action.lua')
script_system_dofile('ui_renderer.lua')
script_system_dofile('event_system.lua')
script_system_dofile('input_manager.lua')

script_system_dofile('addon_manager.lua')

script_system_dofile('module/team.lua')

SERVER_HOST = command_arg_opt_str('host','127.0.0.1')
SERVER_PORT = command_arg_opt_int('port', 45000)
DBG_PORT = command_arg_opt_int('dbg_port', 9600)
luadbg_listen(DBG_PORT)

function main()
	window_system_init(SCREEN_WIDTH,SCREEN_HEIGHT)
	window_system_show()	
end

function on_script_system_init()
    content_system_init()
    net_manager_init(SERVER_HOST, SERVER_PORT)
    text_renderer_init()
    timer_manager_init()
    input_manager_init()
    resource_manager_init()
    sprite_renderer_init()
    imgui_init()
    scene_manager_init()
	actor_manager_init()
	-- combat_system_init()
	asm_system_init()
    load_all_addons()
end

function on_script_system_update()
	input_manager_update()
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


main()