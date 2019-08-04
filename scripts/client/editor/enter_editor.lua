local AccountSB = imgui.CreateStrbuf('Ocean藏心',256)
local PosX = imgui.CreateStrbuf('200',128)
local PosY = imgui.CreateStrbuf('2790',128)
local show_demo = false

function on_enter_editor_update()    
    imgui.Begin('Splash')
    local res 
    ret, show_demo = imgui.Checkbox('Demo', show_demo)
    if show_demo then
        imgui.ShowDemoWindow(show_demo)
    end

    imgui.Text("Nickname   :");
	imgui.SameLine();
	imgui.InputText("##account", AccountSB);
    
    imgui.Text("Pos :");
    imgui.SameLine()
    imgui.PushItemWidth(80)
    imgui.InputText("##posX", PosX)
    imgui.SameLine()
    imgui.InputText("##posY", PosY)
    imgui.PopItemWidth()
    
    if imgui.Button("Sigin Up") then 
    end
    imgui.SameLine();
    if imgui.Button("Sigin In") then
    end

    if imgui.Button("EnterGame") then 
        scene_manager_switch_scene_by_name(DefaultSceneName)
        local pos_x = tonumber(PosX:str())
        local pos_y = tonumber(PosY:str())
        scene_send_login_message( AccountSB:str(),1070,  pos_x,pos_y,1 ,40,1)
    end
    
    if imgui.Button("EnterBattle") then
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
    imgui.End()
end


