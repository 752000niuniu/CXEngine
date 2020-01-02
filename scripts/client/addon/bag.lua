local ui_is_show_bag = false
function ui_toggle_show_bag()
    ui_is_show_bag = not ui_is_show_bag
    return ui_is_show_bag
end


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

function ui_show_bag()
    if not ui_is_show_bag then return end
    local player = actor_manager_fetch_local_player()
    if not player then return end
    imgui.Begin('Bag')
    if imgui.Button('客户端重载') then
        actor_manager_clear_all()
        script_system_dofile('../share/enums.lua')
        script_system_dofile('actor_metatable.lua')
        script_system_dofile('../share/actor_metatable.lua')
        script_system_dofile('../share/utils.lua')
        script_system_dofile('../combat/combat_system.lua')
        combat_system_init()
        script_system_dofile('editor/imgui_editor.lua')
        script_system_dofile('ui_renderer.lua')
        script_system_dofile('input_manager.lua')
        scene_manager_reload()
        game_map_reset_map_offset()
        script_system_dofile('input_manager.lua')
        script_system_dofile('addon_manager.lua')
        load_all_addons()

        script_system_dofile('module/team.lua')
        script_system_dofile('module/dialog.lua')

        game_server_on_connection(true)
        
        collectgarbage()
    end
    imgui.SameLine()
    if imgui.Button('服务端重载') then
        local msg = {code = [[
            server_reload()
        ]]} 
        net_send_message(PTO_C2S_DOSTRING, cjson.encode(msg) ) 
    end
    imgui.SameLine()
    if imgui.Button('刷新数据库') then
        net_send_message(PTO_C2C_SAVE_ACCOUNT_DATABASE,cjson.encode({}))
        net_send_message(PTO_C2C_SAVE_PLAYER_DATABASE,cjson.encode({}))
    end
    imgui.SameLine()
    if imgui.Button('刷新角色') then
        local player = actor_manager_fetch_local_player()
        if player then
            net_send_message(PTO_C2S_CREATE_PLAYER,cjson.encode(player:GetProperties()))
        end
    end

    if imgui.Button('创建队伍') then
        local player = actor_manager_fetch_local_player()
        if player then
            player:CreateTeam()
        end
    end
    imgui.SameLine()
    if imgui.Button('离开队伍') then
        local player = actor_manager_fetch_local_player()
        if player then
            player:DismissTeam()
        end
    end


    if imgui.CollapsingHeader('MyPal') then
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
    end
    
    if imgui.CollapsingHeader('AvatarRole') then
        local avatar_role_tbl = content_system_get_table('role')    
        local role_keys = fetch_role_keys(avatar_role_tbl)
        imgui_std_horizontal_button_layout(avatar_role_tbl,gen_next_sortk_fn(avatar_role_tbl), function(k,v)
            net_manager_player_dostring(string.format([[
                player:SetProperty(PROP_ACTOR_TYPE,ACTOR_TYPE_PLAYER)
                player:SetProperty(PROP_AVATAR_ID, '%s') 
                player:SetProperty(PROP_WEAPON_AVATAR_ID,'')
            ]],k))
        end)
    end

    if imgui.CollapsingHeader('AvatarWeapon') then
        local avatar_weapon_tbl =  content_system_get_table('weapon')    
        local avatar_key = player:GetProperty(PROP_AVATAR_ID)
        local keys = fetch_weapon_keys(avatar_weapon_tbl,avatar_key)
        imgui_std_horizontal_button_layout(avatar_weapon_tbl,custom_gen_next_sortk_fn(avatar_weapon_tbl,keys), function(k,v)
            net_manager_player_dostring(string.format([[
                player:SetProperty(PROP_WEAPON_AVATAR_ID,'%s')
            ]],k))
        end)

        if imgui.Button('DropWeapon') then
            net_manager_player_dostring([[
                player:SetProperty(PROP_WEAPON_AVATAR_ID,'')
            ]])
        end
    end
    if imgui.CollapsingHeader('门派') then
        local school = content_system_get_table('school')
        imgui.HorizontalLayout(school,next,function(k,v) 
            if imgui.Button(v.name..'##'..v.ID) then
                net_manager_player_dostring(string.format([[ player:SetProperty(PROP_SCHOOL, %d) ]],v.ID))
            end
        end)
    end

    if imgui.CollapsingHeader('CMD') then
        if imgui.Button('满血') then
            net_manager_player_dostring(string.format([[ 
                player:SetProperty(PROP_HP, %d) 
            ]], 10000 ))
        end

        if imgui.Button('升级') then
            net_manager_player_dostring(string.format([[ 
                player:SetProperty(PROP_LV, 100) 
                player:SetProperty(PROP_HP,1000) 

                player:SetProperty(PROP_BASE_FORCE,200) 
                player:SetProperty(PROP_BASE_HEALTH,200) 
                player:SetProperty(PROP_BASE_STAMINA,200) 
                player:SetProperty(PROP_BASE_AGILITY,200) 
                player:SetProperty(PROP_BASE_MAGIC,200) 
            ]]))
        end

        if imgui.Button('同步位置') then
            game_map_reset_map_offset()
            net_manager_player_dostring(string.format([[ 
                player:SetProperty(PROP_POS, {305,441}) 
                player:SetProperty(PROP_SCENE_ID, %d) 
            ]], scene_manager_get_current_scene_id()))
        end
      
        if imgui.Button('重连服务器') then
            net_manager_reconnect()
        end
    end

    imgui.End()
end
