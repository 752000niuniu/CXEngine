local cjson = require("cjson")
local imgui = require("imgui")

function OnSceneInit()
    local s  = "Ocean藏心"
--     scene_add_player(s, 200,2790,1,9,5)
--     scene_add_npc(s..tostring(1), 530,1110,1,1,1,"今天天气不错#51")
--     scene_add_pet(s..tostring(2), 560,970,1,5,1)
--     scene_add_npc(s..tostring(3), 530,1110,1,30,1,
-- [[    该怎么去形容你最贴切
--     拿什么跟你作比较才算特别
--     对你的感觉 强烈
-- ]])
--     scene_add_player_by_templ_name("骨精灵_wand",1)
--     -- scene_add_player(s..tostring(1), 530,1110,1,56)
--     -- scene_add_player(s..tostring(2), 530,1110,2,42)
--     -- scene_add_player(s..tostring(3), 530,1110,3,50)
--     -- scene_add_player(s..tostring(4), 530,1110,4,2)
    scene_set_announcement(
        [[项目地址：github.com/oceancx/SimpleEngine
        C++/OpenGL/Lua纯手工游戏编程
        有问题随时提问，QQ群：465885387]])
    scene_set_chat(
        [[游戏程序员下班日常#109]]
    )
end

local move_speed = 400.0
local frame_speed = 0.0
local s_Account = 'oceancx11'
local PosXY = '200,2790'
function OnSceneUpdate()
    -- imgui.Text("TestImGUI")
    -- imgui.Begin("[Scene22]", 0, 0);
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
