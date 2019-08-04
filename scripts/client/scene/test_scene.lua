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

function OnSceneUpdate()
end 

function OnSceneDraw()

end
