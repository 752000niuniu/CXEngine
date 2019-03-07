
function on_test_scene_load()
end

function on_test_scene_unload()
end

local s_Account = 'oceancx11'
local s_Password ='200,2790'
local TextBuffSize = 1024

local move_speed = 400
local frame_speed = 0
function on_test_scene_update(dt)
    local res, text 
	imgui.Text("Nickname   :");
    imgui.SameLine();
    s_Account =  imgui.InputText("##account", s_Account,TextBuffSize, 0)
	imgui.Text("Pos :");
    imgui.SameLine();
    s_Password =  imgui.InputText("##password", s_Password,TextBuffSize, 0)
	if imgui.Button("Sigin Up") then
        b = 1
    end
    imgui.SameLine();
    if imgui.Button("Sigin In") then
        b = 2
    end
	
    if imgui.Button("EnterGame") then
        scene_manager_switch_scene_by_name('门派_方寸山全景')
    end

    if (imgui.Button("EnterBattle")) then 
        scene_manager_switch_scene_by_name('BattleScene')
    end

    if imgui.Button('Reload') then 
        -- script_system_dofile('scene/test_net_scene.lua')
        script_system_dofile('utils.lua')
        script_system_dofile('scene/test_net_scene.lua')
        
        debug.sethook()
        script_system_dofile('debugger.lua')
    end

    if imgui.Button('printEnv') then
        utils_dump_table(_ENV)
    end

   


end 