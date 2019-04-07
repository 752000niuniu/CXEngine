-- script_system_dofile 'scene/scene.lua'
-- script_system_dofile('scene/test_net_scene.lua')
-- script_system_dofile('scene/animation_editor.lua')
-- DefaultSceneName = "门派_方寸山全景"
-- DefaultSceneName = "UIScene"
-- DefaultSceneName = "WASViewer"
DefaultSceneName = "TestNetScene"
  
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
local current_scene_name = 'default'

    
function on_scene_manager_init()
    local parsed_tsv = utils_parse_tsv_file_as_table(fs_get_tsv_path('map'),false)
    for i,row in ipairs(parsed_tsv) do
        scene_manager_add_scene(tonumber( row.ID), row.name)
    end

    scene_manager_add_scene(-1, "BattleScene");
	scene_manager_add_scene(-100, "Splash");
	scene_manager_add_scene(-101, "WASViewer");
	scene_manager_add_scene(-102, "UIScene");
	scene_manager_add_scene(-103, "TestScene");
	scene_manager_add_scene(-104, "default");
    scene_manager_add_scene(-105, "AnimationEditor");


    -- m_MapTSV(utils::tsv(FileSystem::GetTSVPath("map"))),

    -- for i=1,10,1 do
    --     scene_add_npc(19,0x49386FCE , s.. tostring(i), x,y,0,3,15, sayings[i])
    -- end
    -- scene_set_player("oceancx",10,10,3,15)
    
    for i,v in ipairs(scene_lua_files) do
        local path = lua_file_path(v.file)
        local module = {
            exports = {},
            env = _ENV
        }     
        
        setmetatable(module,{ __index = function(t,k)
            local env = rawget(t, "env")
            local v = rawget(env, k); if v then return v end
            local exports = rawget(t, "exports")
            local v = rawget(exports, k); if v then return v end 
        end})

        local fun,err = loadfile(path,'bt',module)
        if fun then
            fun()
            scene_list[v.name] = module
        end
    end

    scene_manager_switch_scene_by_name(current_scene_name)
end

function on_scene_manager_update()

end

function on_scene_manager_draw()
    
end


function on_scene_init()
    scene_list[current_scene_name].on_scene_init()
end

function on_scene_update()
    scene_list[current_scene_name].on_scene_update()
end 

function on_scene_draw()
    scene_list[current_scene_name].on_scene_draw()
end
