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
    {name='AnimationEditor' ,  file= 'scene/animation_editor.lua'},
    {name='PackageUnpacker' ,  file= 'scene/package_unpacker.lua'},
    {name='BattleScene' ,            file= 'scene/battle_scene.lua'},
    {name='门派_方寸山全景' ,       file= 'scene/fangcunshan_scene.lua'},
    {name='Splash' ,            file= 'scene/splash_scene.lua'},
    {name='TestScene' ,          file= 'scene/test_scene.lua'}
}

local scene_list = {}
local current_scene_name = 'AnimationEditor'
function on_scene_manager_init()
    local parsed_tsv = utils_parse_tsv_file_as_table(fs_get_tsv_path('map'),false)
    for i,row in ipairs(parsed_tsv) do
        scene_manager_add_scene(tonumber(row.ID), row.name)
    end

    scene_manager_add_scene(-1, '门派_方寸山全景')
    scene_manager_add_custom_scene(-2, "BattleScene");
	scene_manager_add_custom_scene(-100, "Splash");
	scene_manager_add_custom_scene(-101, "WASViewer");
	scene_manager_add_custom_scene(-102, "UIScene");
	scene_manager_add_custom_scene(-103, "TestScene");
    scene_manager_add_custom_scene(-105, "AnimationEditor");
    scene_manager_add_custom_scene(-106, "PackageUnpacker");
    
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
        else
            cxlog_info(fun,err)
        end
    end

    for name,v in pairs(scene_list) do
        table.insert(scene_list_name, name)
    end
    scene_manager_switch_scene_by_name(current_scene_name)
end

function on_scene_manager_init_scene(name)
    actor_manager_set_scene(scene_manager_get_current_scene_id())
    scene_list[name].OnSceneInit() 
end

function on_scene_manager_update(name)
    on_enter_editor_update()
    on_scene_editor_update()

    scene_list[name].OnSceneUpdate()
end

function on_scene_manager_draw(name)
    scene_list[name].OnSceneDraw()
end

function scene_manager_reload(name)

    for i,v in ipairs(scene_lua_files) do
        if v.name == name then
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
                scene_list[name].OnSceneInit() 
                return
            else
                cxlog_info(fun,err)
            end
        end
    end
end

local KEY_RELEASE_MOVE_AMOUT = 30
local roleID = 0 
local weaponID = 0 

function fix_input_manager_mouse_pos()
    local wx,wy = imgui.GetWindowPos()
    local vid,x,y  = imgui.GetMainViewport()
    input_manager_set_window_pos(wx-x,wy-y)
end

function on_game_imgui_update(name)
    fix_input_manager_mouse_pos()

    imgui.Dummy(600)
    imgui.SameLine()
    imgui.Text('FPS:'.. math.floor(window_system_get_fps()*1000))
    imgui.SameLine()
    if imgui.Button('RELOAD') then
        scene_manager_reload(name)
        collectgarbage()
        return
    end

    if scene_list[name].OnSceneImGuiUpdate then
        scene_list[name].OnSceneImGuiUpdate()
    end

    local player = actor_manager_fetch_local_player()
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
            player:ChangeRole(roleID)
            roleID = roleID + 1
            if roleID == 12 then
                roleID = 1
            end
        end

        if imgui.IsKeyReleased(string.byte('3')) then
            player:ChangeWeapon(weaponID)
            weaponID = weaponID + 1
            if weaponID == 10 then
                weaponID =0
            end
        end
        
        if imgui.IsWindowFocused() then
            if imgui.IsMouseClicked(0) then
                local mx,my = imgui.GetMousePos()
                local wx,wy = imgui.GetWindowPos()
                mx = mx - wx
                my = my - wy
                local dest_x, dest_y = util_screen_pos_to_map_pos(mx, my)
                player:MoveTo(dest_x,dest_y)

                local msg = {}
                msg.pid = player:GetID()
                msg.x = dest_x
                msg.y = dest_y
                net_send_message(PTO_C2C_MOVE_TO_POS, cjson.encode(msg))
            end
        end
    end
end

