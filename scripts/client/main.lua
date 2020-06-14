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
    content_system_init()
    text_renderer_init()
    timer_manager_init()
    input_manager_init()
    resource_manager_init()
    sprite_renderer_init()
    imgui_init()
    scene_manager_init()
	actor_manager_init()
	combat_system_init()
    load_all_addons()
    net_manager_init(SERVER_HOST, SERVER_PORT)
end

function on_script_system_update()
	input_manager_update()
    net_manager_update()
    timer_manager_update(window_system_get_dt())
    resource_manager_update()
    scene_manager_update()
    return true
end

function on_script_system_draw()
    
    local vid,x,y,w,h = imgui.GetMainViewport()
    imgui.SetNextWindowPos(x,y)
    imgui.SetNextWindowSize(w,h)
    imgui.SetNextWindowViewport(vid)
    
    imgui.PushStyleVar2(ImGuiStyleVar_WindowPadding,0,0)
    imgui.Begin('Game', ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking)
    imgui.PopStyleVar()
    
    scene_manager_draw()
    local cs_x,cs_y = imgui.GetCursorPos()
    local css_x,css_y = imgui.GetCursorScreenPos()
    scene_manager_draw_imgui(css_x,css_y)
    imgui.SetCursorPos(cs_x,cs_y)
    imgui.End()
end

function on_script_system_deinit()
    net_manager_deinit()
    timer_manager_deinit()
    input_manager_deinit()
    resource_manager_deinit()
    scene_manager_deinit()
    actor_manager_deinit()
end

do
    window_system_init(SCREEN_WIDTH,SCREEN_HEIGHT)
    iw_set_font(vfs_get_workdir()..'/res/font/msyhl.ttc')

	window_system_show()	
end