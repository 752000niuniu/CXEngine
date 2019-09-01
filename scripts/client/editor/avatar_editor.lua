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
local COLOR_SCHEMES 

local COLOR_SCHEMES_SELECT_MAP = {}
function update_color_schemes_select_map()
    COLOR_SCHEMES_SELECT_MAP = {}
    for i=1,#COLOR_SCHEMES.segments-1 do
        COLOR_SCHEMES_SELECT_MAP[i] = 1
    end
end

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

    if imgui.CollapsingHeader('Players') then
        local players = actor_manager_fetch_all_players()
        imgui.HorizontalLayout(players,next,function(k,v) 
            if imgui.Button(v:GetAvatarID()..'##'..v:GetID()) then
                actor_manager_set_local_player(v:GetID())
            end
        end)
        if imgui.Button('Clear') then
            actor_manager_clear_all()
        end
    end  

    if imgui.CollapsingHeader('NPCTemplate') then
        local npc_tbl = content_system_get_table('npc')
        imgui.HorizontalLayout(npc_tbl,next,function(k,v) 
            if imgui.Button(v.ID) then
                local actor = actor_manager_create_player(os.time())
                actor:SetType(ACTOR_TYPE_PET)
                actor:SetAvatarID(v.ID)
                actor:SetWeaponAvatarID('')
                actor:SetPos(player:GetPos())
                actor:SetSceneID(player:GetSceneID())
                actor:SetDir(player:GetDir())
            end
        end)
    end  


    if imgui.CollapsingHeader('AvatarRole') then
        local avatar_role_tbl = content_system_get_table('role')    
        local role_keys = fetch_role_keys(avatar_role_tbl)
        imgui_std_horizontal_button_layout(avatar_role_tbl,gen_next_sortk_fn(avatar_role_tbl), function(k,v)
            player:SetType(ACTOR_TYPE_PLAYER)
            player:SetAvatarID(k)
            player:SetWeaponAvatarID('')
        end)
    end

    if imgui.CollapsingHeader('MyPal') then
        local files = vfs_list_files(vfs_get_tablepath('wasee_pal'))
        local filenames = {}
        for k,f in pairs(files) do
            local  name = f:match('wasee_pal/(.+)%.')
            name = util_gb2312_to_utf8(name)
            table.insert(filenames,name)
        end

        
        imgui.HorizontalLayout(filenames,next,function(k,v)
            if imgui.Button(v) then
                select_pal = files[k]
                COLOR_SCHEMES = decode_mypal(select_pal)
                update_color_schemes_select_map()
                cxlog_info('...',cjson.encode(COLOR_SCHEMES))
                cxlog_info('...',cjson.encode(COLOR_SCHEMES_SELECT_MAP))
            end
        end)

        if COLOR_SCHEMES then
            for i=1,#COLOR_SCHEMES.segments-1 do
                imgui.BeginGroup()
                local title = COLOR_SCHEMES.segments[i]..'-'..COLOR_SCHEMES.segments[i+1]
                imgui.AlignTextToFramePadding()
                imgui.Text(title)
                
                for mat_i,mat in ipairs(COLOR_SCHEMES[i]) do
                    local selected = imgui.RadioButton(mat_i..'##'..title ,mat_i == COLOR_SCHEMES_SELECT_MAP[i])
                    if selected then
                        COLOR_SCHEMES_SELECT_MAP[i] = mat_i
                        --选择矩阵 重新load actor的avatar
                        local new_pal = {}
                        new_pal.segments = COLOR_SCHEMES.segments
                        for seg_i=1,#COLOR_SCHEMES.segments-1 do
                            local mat_i = COLOR_SCHEMES_SELECT_MAP[seg_i]
                            local seg_mats = COLOR_SCHEMES[seg_i]
                            local mat  = seg_mats[mat_i]
                            local seg_pal = {}
                            seg_pal.from = COLOR_SCHEMES.segments[seg_i]
                            seg_pal.to = COLOR_SCHEMES.segments[seg_i+1]
                            seg_pal.mat = mat
                            table.insert(new_pal, seg_pal)
                        end
                        cxlog_info('new_pal  '.. cjson.encode(new_pal))
                        player:ChangePalMatrix(new_pal)
                    end
                end
                                
                imgui.EndGroup()
                if i ~= #COLOR_SCHEMES.segments-1 then
                    imgui.SameLine()
                end
            end
        end
        

    end
    
    if imgui.CollapsingHeader('AvatarWeapon') then
        local avatar_weapon_tbl =  content_system_get_table('weapon')    
        local avatar_key = player:GetAvatarID()
        local keys = fetch_weapon_keys(avatar_weapon_tbl,avatar_key)
        imgui_std_horizontal_button_layout(avatar_weapon_tbl,custom_gen_next_sortk_fn(avatar_weapon_tbl,keys), function(k,v)
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
        imgui_std_horizontal_button_layout(magic_tbl,gen_next_sortk_fn(magic_tbl),function(k,v) 
            cxlog_info(k,v)
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