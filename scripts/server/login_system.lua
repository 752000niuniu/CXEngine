

stub[PTO_C2C_LOGIN] = function(req)
	print('pto_c2s_login req', cjson.encode(req))
    local req_player = actor_manager_create_actor(req.pid)
    req_player:SetProperty(PROP_NAME, req.name)
    req_player:SetProperty(PROP_SCENE_ID,req.scene_id)
    req_player:SetProperty(PROP_ROLE_ID,req.role_id)
    req_player:SetProperty(PROP_WEAPON_ID,req.weapon_id)
    req_player:SetPos(req.x,req.y)
    cxlog_info('GetID ', req_player:GetID())
    
    local actors = actor_manager_fetch_all_players()    
    -- print('players', #players)
    for i,actor in ipairs(actors) do
        local pid = actor:GetID()
        if pid == req.pid then
            local actors_props = {}
            for _,actor in ipairs(actors) do
                table.insert(actors_props, actor:GetProperties())
            end
            net_send_message(pid,PTO_C2C_PLAYER_ENTER, cjson.encode({local_pid = req.pid, actors = actors_props}))
        else
            net_send_message(pid,PTO_C2C_PLAYER_ENTER, cjson.encode({ actors = { req_player:GetProperties() }}))
        end
    end
end

