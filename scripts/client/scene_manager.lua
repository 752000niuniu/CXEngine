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

function on_scene_manager_init()
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
            local res = fun()
            local x = 'xx'
        end
    end

    -- scene_manager_switch_scene_by_name(DefaultSceneName)
end

function on_scene_manager_update()

end

function on_scene_manager_draw()
    
end
