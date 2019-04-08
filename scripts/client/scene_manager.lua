-- script_system_dofile 'scene/scene.lua'
-- script_system_dofile('scene/test_net_scene.lua')
-- script_system_dofile('scene/animation_editor.lua')
DefaultSceneName = "门派_方寸山全景"
-- DefaultSceneName = "UIScene"
-- DefaultSceneName = "WASViewer"
  
local scene_lua_files = 
{
    {name='default' ,           file= 'scene/scene.lua'},
    {name='animation_editor' ,  file= 'scene/animation_editor.lua'},
    {name='battle' ,            file= 'scene/battle_scene.lua'},
    {name='fangcunshan' ,       file= 'scene/fangcunshan_scene.lua'},
    {name='splash' ,            file= 'scene/splash_scene.lua'},
    {name='test_net' ,          file= 'scene/test_net_scene.lua'}
}

local scene_list = {}
local current_scene_name = 'splash'
    
function on_scene_manager_init()
    local parsed_tsv = utils_parse_tsv_file_as_table(fs_get_tsv_path('map'),false)
    for i,row in ipairs(parsed_tsv) do
        scene_manager_add_scene(tonumber(row.ID), row.name)
    end

    scene_manager_add_scene(-1, current_scene_name)
    scene_manager_add_custom_scene(-2, "BattleScene");
	scene_manager_add_custom_scene(-100, "Splash");
	scene_manager_add_custom_scene(-101, "WASViewer");
	scene_manager_add_custom_scene(-102, "UIScene");
	scene_manager_add_custom_scene(-103, "TestScene");
    scene_manager_add_custom_scene(-105, "AnimationEditor");
    
    for i,v in ipairs(scene_lua_files) do
        local path = lua_file_path(v.file)
        local module = {
            exports = {},
            env = _ENV
        } 
        scene_list[v.name] = module

        setmetatable(module,{ __index = function(t,k)
            local env = rawget(t, "env")
            local v = rawget(env, k); if v then return v end
            local exports = rawget(t, "exports")
            local v = rawget(exports, k); if v then return v end 
        end})

        local fun,err = loadfile(path,'bt',module)
        if fun then
            fun()
        end
    end

    scene_manager_switch_scene_by_name(current_scene_name)
    
end

function on_scene_manager_init_scene()
    scene_list[current_scene_name].OnSceneInit() 
end

function on_scene_manager_update()
    scene_list[current_scene_name].OnSceneUpdate()
end

function on_scene_manager_draw()
    scene_list[current_scene_name].OnSceneDraw()
end

