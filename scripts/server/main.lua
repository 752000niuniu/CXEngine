

script_system_dofile('../share/vfs.lua')
script_system_dofile('../share/utils.lua')


function on_script_system_init()
    game_server_start(45000)
end

function on_script_system_update()
    game_server_update()
    return true
end


function on_script_system_deinit()
    game_server_stop()
end

function pto_c2s_login(req)
    local req_player = actor_manager_create_player(req.pid)
    req_player:SetName(req.name)
    req_player:SetSceneID(req.scene_id)
    req_player:SetRoleID(req.role_id)
    req_player:SetWeaponID(req.weapon_id)
    req_player:SetX(req.x)
    req_player:SetY(req.y)
    req.is_local = true
    local players = actor_manager_fetch_all_players()    
    print("players", #players)
    for k,player in pairs(players) do
        local pid = player:GetID()        
        if req.pid == pid then
            local pinfos = {}
            table.insert(pinfos, req)
            for _, other in pairs(players) do
                if other.pid ~= req.pid then
                    local pinfo = {}
                    pinfo.pid = other:GetID()
                    pinfo.name = other:GetName()
                    pinfo.scene_id = other:GetSceneID()
                    pinfo.role_id = other:GetRoleID()
                    pinfo.weapon_id = other:GetWeaponID()
                    pinfo.x = other:GetX()
                    pinfo.y = other:GetY()
                    table.insert(pinfos, pinfo)
                end
            end
            print(cjson.encode(pinfos))
            net_send_message(pid,PTO_S2C_PLAYER_ENTER, cjson.encode(pinfos))        
        else
            print(cjson.encode({req}))
            net_send_message(pid,PTO_S2C_PLAYER_ENTER, cjson.encode({req}))
        end        
    end
end

function game_server_dispatch_message(pt)
    local type = pt:ReadAsInt()
    local js = pt:ReadAllAsString()
    print('game_server_dispatch_message' , type, js)
    local req = cjson.decode(js)
    if  type == PTO_C2S_LOGIN then
        pto_c2s_login(req)
    elseif type == PTO_C2S_LOGOUT then
        
    elseif type == PTO_C2S_MOVE_TO_POS then
        -- net_send_message_to_all_players(PTO_C2S_MOVE_TO_POS,msgjs)
    elseif type == PTO_C2S_CHAT then
        -- net_send_message_to_all_players(PTO_C2S_CHAT,msgjs)
    end
end