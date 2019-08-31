local DIR_NAMES = {
    [6] = 'DIR_N',
    [4] = 'DIR_S',
    [5] = 'DIR_W',
    [7] = 'DIR_E',
    [3] = 'DIR_NE',
    [2] = 'DIR_NW',
    [0] = 'DIR_SE',
    [1] = 'DIR_SW'
}

function fetch_role_keys(tbl)
    local role_keys = {}
    for k, v in pairs(tbl) do
        table.insert(role_keys,k)
    end
    table.sort(role_keys)
    return role_keys
end

function fetch_weapon_keys(tbl, avatar_key)
    local weapon_keys = {}
    local strs= utils_string_split(avatar_key,'-')
    local role_key , weapon_key  = strs[1], strs[2]
    for k, v in pairs(tbl) do
        if role_key == v.role and weapon_key == v.type then
            table.insert(weapon_keys,k)
        end
    end
    table.sort(weapon_keys)
    return weapon_keys
end

function on_avatar_editor_update()
    local player = actor_manager_fetch_local_player()
    if not player then return end
    imgui.Begin('AvatarEditor')

    if imgui.CollapsingHeader('AvatarRole') then
        local avatar_role_tbl =  content_system_get_table('role')    
        local role_keys = fetch_role_keys(avatar_role_tbl)
        for i,k in ipairs(role_keys) do
            if imgui.Button(k) then
                player:SetAvatarID(k)
                player:SetWeaponAvatarID('')
            end
            if i%6~=0 then
                imgui.SameLine()
            end
        end
    end
    if imgui.CollapsingHeader('AvatarWeapon') then
        local avatar_weapon_tbl =  content_system_get_table('weapon')    
        local weapon_keys = fetch_weapon_keys(avatar_weapon_tbl, player:GetAvatarID())
        for i,k in ipairs(weapon_keys) do
            if imgui.Button(k) then
                player:SetWeaponAvatarID(k)
            end
            if i%3~=0 then
                imgui.SameLine()
            end
        end
        if imgui.Button('DropWeapon') then
            player:SetWeaponAvatarID('')
        end
    end

    if imgui.CollapsingHeader('PlayAction') then
        for i=0,action_system_get_action_size()-1 do
            if imgui.Button(action_system_get_action_name(i)) then
                player:SetActionID(i)
            end
            if i%4~=0  then
                imgui.SameLine()
            end
        end
    end

    if imgui.CollapsingHeader('PlayerDir') then
        for i=0,7 do
            if imgui.Button(DIR_NAMES[i]) then
                player:SetDir(i)
            end
        end
    end

    if imgui.CollapsingHeader('PlayerBattle') then
        if imgui.Button('Attack') then
            local players = actor_manager_fetch_all_players()
            for i,v in ipairs(players) do
                if not v:IsLocal()  then
                    player:PlayAttack(v)
                    break
                end
            end
        end
        local magic_tbl = content_system_get_table('magic')
        local index = 1
        for k, v in pairs(magic_tbl) do
            if imgui.Button(k) then
                local id = v.resid
                local players = actor_manager_fetch_all_players()
                for i,v in ipairs(players) do
                    if not v:IsLocal() then
                        player:PlayCast(v,id)
                        break
                    end
                end
            end
            if index % 4== 0 then
                imgui.SameLine()
            end
            index = index + 1
        end
    end

    
    imgui.End()
end