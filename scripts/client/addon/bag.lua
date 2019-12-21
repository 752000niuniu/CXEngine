local ui_is_show_bag = false
function ui_toggle_show_bag()
    ui_is_show_bag = not ui_is_show_bag
    return ui_is_show_bag
end


function ui_show_bag()
    if not ui_is_show_bag then return end
    local player = actor_manager_fetch_local_player()
    if not player then return end
    imgui.Begin('Bag')
    if imgui.CollapsingHeader('AvatarWeapon') then
        local avatar_weapon_tbl =  content_system_get_table('weapon')    
        local avatar_key = player:GetProperty(PROP_AVATAR_ID)
        local keys = fetch_weapon_keys(avatar_weapon_tbl,avatar_key)
        imgui_std_horizontal_button_layout(avatar_weapon_tbl,custom_gen_next_sortk_fn(avatar_weapon_tbl,keys), function(k,v)
            player:SetProperty(PROP_WEAPON_AVATAR_ID,k)
        end)

        if imgui.Button('DropWeapon') then
            player:SetProperty(PROP_WEAPON_AVATAR_ID,'')
        end
    end
    if imgui.CollapsingHeader('Item') then
        if imgui.Button('满血') then
            player:SetProperty(PROP_HP, player:GetMaxHP())
        end

        if imgui.Button('初始化位置') then
            game_map_reset_map_offset()
            player:SetProperty(PROP_POS, {305,441})
            
        end
    end

    imgui.End()
end
