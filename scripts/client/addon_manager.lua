function load_all_addons()
    script_system_dofile('addon/bag.lua')
    script_system_dofile('addon/props.lua')
    script_system_dofile('addon/hud.lua')
    script_system_dofile('addon/quest.lua')
    script_system_dofile('addon/action.lua')
end


function check_dest_hit_actor(dest_x, dest_y)
    local hit_actor = nil
    local actors = actor_manager_fetch_all_players()
    for i,actor in ipairs(actors) do
        -- cxlog_info('check_dest_hit_actor', actor:GetProperty(PROP_NAME))
        if not actor:IsLocal() then
            local avatar = actor:GetAvatar()
            if not avatar then return end
            local avx, avy = actor:GetPos()
            local cx =  avx - avatar:GetFrameKeyX()
            local cy =  avy - avatar:GetFrameKeyY()
            local brx = avatar:GetFrameWidth()
            local bry =  avatar:GetFrameHeight()
            if dest_x >= cx and dest_x <= cx+brx and dest_y >= cy and dest_y <=cy+bry  then
                hit_actor = actor
                break
            end
        end
        
    end
    return hit_actor
end

function addon_manager_imgui_update()
    if imgui.KeysMod('ALT') and imgui.IsKeyPressed(string.byte('E')) then
		ui_toggle_show_bag()
	end

	if imgui.KeysMod('ALT') and imgui.IsKeyPressed(string.byte('W')) then
		ui_toggle_show_props()
    end

    if imgui.KeysMod('ALT') and imgui.IsKeyPressed(string.byte('Q')) then
		ui_toggle_show_quest()
    end

    if imgui.KeysMod('ALT') and imgui.IsKeyPressed(string.byte('D')) then
		ui_toggle_show_action()
    end
    
    ui_show_bag()
    ui_show_props()
    ui_hud_update()
    ui_show_quest()
    ui_show_action()


    if not imgui.IsAnyItemHovered() then
        if imgui.IsMouseClicked(0) then
            local player = actor_manager_fetch_local_player()
            if not player then return end
            if player:GetProperty(PROP_IS_COMBAT) then return end
            local mx,my = input_manager_get_mouse_pos()
            local dest_x, dest_y = util_screen_pos_to_map_pos(mx, my)
            local hit_actor = check_dest_hit_actor(dest_x,dest_y)
            if not hit_actor and not npc_dialog_is_show()  then
                player:MoveTo(dest_x,dest_y)
                local msg = {}
                msg.pid = player:GetID()
                msg.x = dest_x
                msg.y = dest_y
                net_send_message(PTO_C2C_MOVE_TO_POS, cjson.encode(msg))
            else
           
            end
        end
    end

end