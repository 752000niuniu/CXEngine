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
        local avatar_role_tbl = content_system_get_table('role')    
        local role_keys = fetch_role_keys(avatar_role_tbl)
        imgui_std_horizontal_button_layout(avatar_role_tbl,function(t,st)
            st = st==nil and 1 or (st+1)
            if st>#role_keys then return nil end
            return st,t[st],role_keys[st]
        end, function(k,v)
            player:SetAvatarID(k)
            player:SetWeaponAvatarID('')
        end)
    end
    if imgui.CollapsingHeader('AvatarWeapon') then
        local avatar_weapon_tbl =  content_system_get_table('weapon')    
        local avatar_key = player:GetAvatarID()
        local keys = fetch_weapon_keys(avatar_weapon_tbl,avatar_key)
        imgui_std_horizontal_button_layout(avatar_weapon_tbl,function(t,st)
            st = st==nil and 1 or (st+1)
            if st>#keys then return nil end
            return st,keys[st],keys[st]
        end, function(k,v)
            player:SetWeaponAvatarID(k)
        end)

        if imgui.Button('DropWeapon') then
            player:SetWeaponAvatarID('')
        end
    end

    if imgui.CollapsingHeader('PlayAction') then
        imgui_std_horizontal_button_layout({},function(t, st)
            st = st==nil and 0 or (st+1) 
            if st == action_system_get_action_size() then return nil end
            return st, st, action_system_get_action_name(st)
        end, 
        function(k,v)
            player:SetActionID(v)
        end)
    end

    if imgui.CollapsingHeader('PlayerDir') then
        imgui_std_horizontal_button_layout(DIR_NAMES,function(t, st)
            st = st==nil and 0 or (st+1) 
            if st == 8 then return nil end
            return st, st, t[st]
        end, 
        function(i,v)
            player:SetDir(v)
        end)
    end

    if imgui.CollapsingHeader('PlayerBattle') then
        local cx,cy = imgui.GetCursorPos()
        if imgui.Button('Attack') then
            local players = actor_manager_fetch_all_players()
            for i,v in ipairs(players) do
                if not v:IsLocal() then
                    player:PlayAttack(v)
                    break
                end
            end
        end

        local magic_tbl = content_system_get_table('magic')
        local keys = utils_fetch_sort_keys(magic_tbl)
        imgui_std_horizontal_button_layout(magic_tbl,function(t, st)
            st = st==nil and 1 or (st+1) 
            if st > #keys then return nil end
            return st, t[keys[st]], keys[st]
        end,  
        function(k,v) 
            local id = v.resid
            local players = actor_manager_fetch_all_players()
            for i,v in ipairs(players) do
                if not v:IsLocal() then
                    player:PlayCast(v,id)
                    break
                end
            end
        end)
    end

    
    imgui.End()
end