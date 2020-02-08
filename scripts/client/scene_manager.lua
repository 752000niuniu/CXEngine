scene_list_name = {}

local scene_lua_files = 
{
    {name='PackageUnpacker' ,  file= 'scene/package_unpacker.lua'},
    {name='BattleScene' ,            file= 'scene/battle_scene.lua'},
    {name='东海湾全景' ,       file= 'scene/fangcunshan_scene.lua'},
    {name='Splash' ,            file= 'scene/splash_scene.lua'},
    {name='TestScene' ,          file= 'scene/test_scene.lua'}
}

local scene_list = {}
local current_scene_name = 'Splash'
function on_scene_manager_init()
    local parsed_tsv = utils_parse_tsv_file_as_table(fs_get_tsv_path('map'),false)
    for i,row in ipairs(parsed_tsv) do
        scene_manager_add_scene(tonumber(row.ID), row.name)
    end

    scene_manager_add_scene(-1, '东海湾全景')
    -- scene_manager_add_custom_scene(-2, "BattleScene", 1506);
	scene_manager_add_custom_scene(-100, "Splash", 1506);
	-- scene_manager_add_custom_scene(-101, "WASViewer", 1506);
	-- scene_manager_add_custom_scene(-102, "UIScene", 1506);
	-- scene_manager_add_custom_scene(-103, "TestScene", 1506);
    -- scene_manager_add_custom_scene(-106, "PackageUnpacker", 1506);
    
    for i,v in ipairs(scene_lua_files) do
        local path = vfs_get_luapath(v.file)
        local module = {
            exports = {},
            env = _ENV
        } 
        scene_list[v.name] = module

        setmetatable(module,{ __index = function(t,k)
            local env = rawget(t, "env")
            local v = env[k] ; if v then return v end
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
    -- scene_manager_switch_scene_by_name(current_scene_name)
end

function on_scene_manager_init_scene(name)
    current_scene_name = name
    if scene_list[name] then
        scene_list[name].OnSceneInit() 
    end
end

function on_scene_manager_update(name)
    if scene_list[name] then
        scene_list[name].OnSceneUpdate()
    end
end

function on_scene_manager_draw(name)
    if scene_list[name] then
        scene_list[name].OnSceneDraw()
    end
end

function scene_manager_reload(name)
    name = name or current_scene_name
    for i,v in ipairs(scene_lua_files) do
        if v.name == name then
            local path = vfs_get_luapath(v.file)
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
function fix_input_manager_mouse_pos()
    local wx,wy = imgui.GetWindowPos()
    local vid,x,y  = imgui.GetMainViewport()
    input_manager_set_window_pos(wx-x,wy-y)
end

function on_game_imgui_update(name)
    fix_input_manager_mouse_pos()
    
    imgui.Text('FPS:'.. math.floor(window_system_get_fps()*1000))

    if scene_list[name] then
        if scene_list[name].OnSceneImGuiUpdate then
            scene_list[name].OnSceneImGuiUpdate()
        end
        if scene_is_combat() then
            combat_system_imgui_update()
        end
    end

    local actors = actor_manager_fetch_all_actors()
    for i,actor in ipairs(actors) do
        if actor:GetProperty(PROP_SHOW_BOUNDINGBOX) then
            actor:DrawBoundingBox()
        end
        if actor:GetProperty(PROP_SHOW_AVATAR_INFO) then
            local x ,y  = actor:GetPos()
            local avatar = actor:GetAvatar()
            if not avatar then return end
            imgui.SetCursorPos(x-55,y+50)
            imgui.BeginGroup()
            actor:DrawAvatarInfo()
            imgui.EndGroup()
        end
    end
    addon_manager_imgui_update()
end

stub = net_manager_stub()

stub[PTO_C2C_PLAYER_ENTER] = function(req)
	for i,actor_info in ipairs(req.actors) do
		local actor = actor_manager_create_actor(actor_info[tostring(PROP_ID)])
		cxlog_info('create pid ', actor_info[tostring(PROP_ID)] )
		actor:SetProperties(actor_info)
		-- actor_reg_event(actor, ACTOR_EV_ON_CLICK, actor_ev_on_click)
	end
	if req.local_pid then
		actor_manager_set_local_player(req.local_pid)
        local player = actor_manager_fetch_local_player()
        local scene_id = player:GetProperty(PROP_SCENE_ID)
        cxlog_info('PTO_C2C_PLAYER_ENTER', scene_id)
        scene_manager_switch_scene_by_id(scene_id)
	end
end

stub[PTO_C2C_NPC_ENTER] = function(req)
	for i,actor_info in ipairs(req.npcs) do
		local actor = actor_manager_create_actor(actor_info[tostring(PROP_ID)])
        actor:SetProperties(actor_info)
	end

	local player = actor_manager_fetch_local_player()
	local req = {}
	req.pid = player:GetID()
	net_send_message(PTO_C2S_FETCH_TEAM, cjson.encode(req))
end
