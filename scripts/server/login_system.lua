

stub[PTO_C2C_LOGIN] = function(req)
    local req_player = actor_manager_create_actor(req.pid)
    local props = fetch_player_database_props(req.pid)
    if props then
        req_player:SetProperties(props)
        req_player:SetProperty(PROP_NAME,math.tointeger(req.pid))
    end

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
    
    local actors_props = {}
    local actors = actor_manager_fetch_all_actors()   
    for i,actor in ipairs(actors) do
        if not actor:IsPlayer() then
            table.insert(actors_props, actor:GetProperties())
        end
    end

    net_send_message(req.pid,PTO_C2C_NPC_ENTER, cjson.encode({ npcs = actors_props}))
end

