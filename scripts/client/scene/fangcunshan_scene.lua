
function OnSceneInit()           
 scene_set_chat(
        [[游戏程序员下班日常#109]]
    )
end

function OnSceneUpdate()
    if scene_is_combat() then
        
    else
        if imgui.IsMouseClicked(0) then
            local player = actor_manager_fetch_local_player()
            local mx,my = input_manager_get_mouse_pos()
            local dest_x, dest_y = util_screen_pos_to_map_pos(mx, my)
            local hit_actor = check_dest_hit_actor(dest_x,dest_y)
            if not hit_actor and not npc_dialog_is_show()  then
                player:MoveTo(dest_x,dest_y)
                local msg = {}
                msg.pid = player:GetID()
                msg.x = dest_x
                msg.y = dest_y
                cxlog_info('fangcunshna sOnSceneUpdate' ,  'x' ,msg.x, ' y',msg.y)
                net_send_message(PTO_C2C_MOVE_TO_POS, cjson.encode(msg))
            else
           
            end
        end
    end
end


function OnSceneDraw()

end