

stub[PTO_C2C_LOGIN] = function(req)
	print('pto_c2s_login req', cjson.encode(req))
    local req_player = actor_manager_create_actor(req.pid)
    req_player:SetProperty(PROP_NAME, req.name)
    req_player:SetProperty(PROP_SCENE_ID,req.scene_id)
    req_player:SetProperty(PROP_ROLE_ID,req.role_id)
    req_player:SetProperty(PROP_WEAPON_ID,req.weapon_id)
    req_player:SetProperty(PROP_POS,req.x,req.y)
    local players = actor_manager_fetch_all_players()    
    print('players', #players)
    for k,player in ipairs(players) do
        local pid = player:GetID()        
        cxlog_info('player: ',pid)
        if req.pid == pid then
            local pinfos = {}
            req.is_local = true
            table.insert(pinfos,req)
            local others = actor_manager_fetch_all_players()    
            for _, other in ipairs(others) do
                if other:GetID() ~= pid then
                    local pinfo = {}
                    pinfo.pid = other:GetID()
                    pinfo.name = other:GetProperty(PROP_NAME)
                    pinfo.scene_id = other:GetProperty(PROP_SCENE_ID)
                    pinfo.role_id = other:GetProperty(PROP_ROLE_ID)
                    pinfo.weapon_id = other:GetProperty(PROP_WEAPON_ID) 
                    pinfo.x,pinfo.y  = other:GetProperty(PROP_POS)
                    table.insert(pinfos, pinfo)
                end
            end
            net_send_message(pid,PTO_C2C_PLAYER_ENTER, cjson.encode(pinfos))        
        else
            local pinfos = {}
            req.is_local = false
            table.insert(pinfos,req)
            net_send_message(pid,PTO_C2C_PLAYER_ENTER, cjson.encode(pinfos))
        end        
    end
end
