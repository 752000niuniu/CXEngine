--code of lines b*[^:b#/]+.*$
SCREEN_WIDTH = 800
SCREEN_HEIGHT = 600
DefaultMapID = 1001

script_system_dofile 'utils.lua'
script_system_dofile 'scene_manager.lua'
script_system_dofile 'scene.lua'
script_system_dofile 'table_template.lua'
script_system_dofile 'debugger.lua'

IsCombat = false
-- DefaultSceneName = "门派_方寸山全景"
-- DefaultSceneName = "UIScene"
-- DefaultSceneName = "WASViewer"
DefaultSceneName = "门派_方寸山全景"

--聊天背景框 wzife 39D3BD99 
function main()
    window_system_init(SCREEN_WIDTH,SCREEN_HEIGHT)
    window_system_show()	
end

function on_script_system_init()
    -- generate_avatar_role_tsv()
    debugger_start_session()
    net_manager_init()
    text_renderer_init()
    timer_manager_init()
    input_manager_init()
    resource_manager_init()
    sprite_renderer_init()
    imgui_init()
    scene_manager_init()
end

function on_script_system_update()
    debugger_update_session_new()
    net_manager_update()
    timer_manager_update()
    resource_manager_update()
    scene_manager_update()
end

function on_script_system_draw()
    scene_manager_draw()
end

function on_script_system_deinit()
    -- debugger_stop_session()
    net_manager_deinit()
    timer_manager_deinit()
    input_manager_deinit()
    resource_manager_deinit()
    scene_manager_deinit()
end
