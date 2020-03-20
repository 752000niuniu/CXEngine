SCREEN_WIDTH = 800
SCREEN_HEIGHT = 600
DefaultMapID = 1001

script_system_dofile('../share/enums.lua')
script_system_dofile('../share/enums_protocol.lua')
script_system_dofile('../share/vfs.lua')
script_system_dofile('../share/utils.lua')
script_system_dofile('../share/content_system.lua')
script_system_dofile('../share/actor_metatable.lua')

script_system_dofile 'actor_metatable.lua'

script_system_dofile('net_manager.lua')

script_system_dofile('../combat/combat_system.lua')
script_system_dofile 'scene_manager.lua'
script_system_dofile 'actor_manager.lua'
script_system_dofile('event_system.lua')
script_system_dofile('input_manager.lua')

script_system_dofile('addon_manager.lua')

script_system_dofile('module/team.lua')
script_system_dofile('module/dialog.lua')
script_system_dofile('module/summon.lua')


SERVER_HOST = command_arg_opt_str('host','127.0.0.1')
SERVER_PORT = command_arg_opt_int('port', 45000)
DBG_PORT = command_arg_opt_int('dbg_port', 9600)
luadbg_listen(DBG_PORT)


function on_script_system_init()
   
end

function on_script_system_update()
    gl.clear_color(1.0, 0.8, 0.2, 1.0) -- GLFW orange
    gl.clear("color", "depth")
    imgui.Begin('WINDOW')
    if imgui.Button('OPENGL') then

    end
    imgui.End()
    return true
end

function on_script_system_draw()
	
end

function on_script_system_deinit()
   
end

do
	window_system_init(SCREEN_WIDTH,SCREEN_HEIGHT)
	window_system_show()	
end