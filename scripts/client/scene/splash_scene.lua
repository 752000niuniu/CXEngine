local cjson = require("cjson")
local imgui = require("imgui")



function OnSceneInit()
    local actor1 = lua_new_actor(ACTOR_TYPE_PLAYER, 9)
    local actor2 = lua_new_actor(ACTOR_TYPE_PLAYER, 9)
    actor1:SetProperty('name','oceancx')
    actor2:SetProperty('name','oceancx2')
    
    -- print('name1' , actor1:GetNickName() ,'name2',actor2:GetNickName())
    actor1:Destroy()
    actor2:Destroy()
end

local move_speed = 400.0
local frame_speed = 0.0
local s_Account = 'oceancx11'
local PosXY = '200,2790'
function OnSceneUpdate()
    imgui.Text("Nickname   :");
	imgui.SameLine();
	s_Account = imgui.InputText("##account", s_Account, 1024);
	imgui.Text("Pos :");
	imgui.SameLine();
	PosXY = imgui.InputText("##password", PosXY, 1024);
    if (imgui.Button("Sigin Up")) then 
    end
	imgui.SameLine();
    if (imgui.Button("Sigin In")) then
    end

    if (imgui.Button("EnterGame")) then 
        scene_manager_switch_scene_by_name(DefaultSceneName)
		scene_send_login_message(s_Account,PosXY)
    end
		
	if (imgui.Button("EnterBattle")) then
        scene_manager_switch_scene_by_name('BattleScene')
    end
    
    
    if (imgui.Button("更改贝塞尔")) then
        script_system_dofile 'scene.lua'
        print('更改贝塞尔')
        frame_animation_set_bezier_curve_p1_p2(0.48,0.01,0.01,1.01,1.01)
    end


    local old_speed = frame_speed
    frame_speed =  imgui.DragFloat('FrameSpeed', frame_speed)
    if old_speed ~= frame_speed then
        player_set_frame_speed(frame_speed)
    end
    old_speed = move_speed
    move_speed = imgui.DragFloat('MoveSpeed', move_speed)
    if old_speed~= move_speed then
        player_set_move_speed(move_speed)
    end
end

function OnSceneDraw()

end

