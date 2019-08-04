-- script_system_dofile 'scene/scene.lua'
-- script_system_dofile('scene/test_net_scene.lua')
-- script_system_dofile('scene/animation_editor.lua')
DefaultSceneName = "门派_方寸山全景"
-- DefaultSceneName = "UIScene"
-- DefaultSceneName = "WASViewer"
scene_list_name = {}

script_system_dofile('editor/enter_editor.lua')
script_system_dofile('editor/scene_editor.lua')
local scene_lua_files = 
{
    {name='default' ,           file= 'scene/scene.lua'},
    {name='AnimationEditor' ,  file= 'scene/animation_editor.lua'},
    {name='BattleScene' ,            file= 'scene/battle_scene.lua'},
    {name='门派_方寸山全景' ,       file= 'scene/fangcunshan_scene.lua'},
    {name='Splash' ,            file= 'scene/splash_scene.lua'},
    {name='TestScene' ,          file= 'scene/test_net_scene.lua'}
}

local scene_list = {}
local current_scene_name = '门派_方寸山全景'
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

    for name,v in pairs(scene_list) do
        table.insert(scene_list_name, name)
    end
    scene_manager_switch_scene_by_name(current_scene_name)
end

function on_scene_manager_init_scene(name)
    scene_list[name].OnSceneInit() 
end

function on_scene_manager_update(name)
    on_enter_editor_update()
    on_scene_editor_update()

    scene_list[name].OnSceneUpdate()
end

local KEY_RELEASE_MOVE_AMOUT = 30
function on_game_imgui_update()
    local player = scene_manager_fetch_local_player()
    if player then
        if imgui.IsKeyReleased(string.byte('W') ) then
            player:TranslateY(-KEY_RELEASE_MOVE_AMOUT)
        end

        if imgui.IsKeyReleased(string.byte('A')) then
            player:TranslateX(-KEY_RELEASE_MOVE_AMOUT)
        end

        if imgui.IsKeyReleased(string.byte('S')) then
            player:TranslateY(KEY_RELEASE_MOVE_AMOUT)
        end

        if imgui.IsKeyReleased(string.byte('D')) then
            player:TranslateX(KEY_RELEASE_MOVE_AMOUT)
        end

        if imgui.IsKeyReleased(string.byte('F')) then
            player:TranslateX(KEY_RELEASE_MOVE_AMOUT)
        end

        if imgui.IsKeyReleased(string.byte('1')) then
            player:ChangeAction(0)
        end


        if imgui.IsKeyReleased(string.byte('2')) then
            player:ChangeRole(0)
        end

        if imgui.IsKeyReleased(string.byte('3')) then
            player:ChangeWeapon(0)
        end


    end
    
end

