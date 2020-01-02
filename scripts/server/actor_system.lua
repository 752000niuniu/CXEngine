stub[PTO_C2C_MOVE_TO_POS] = function(req,js)
    local player = actor_manager_fetch_player_by_id(req.pid)
    if not player then return end
    player:SetPos(req.x,req.y)
    net_send_message_to_all_players(PTO_C2C_MOVE_TO_POS,js)
end


stub[PTO_C2C_CHAT] = function(req,js)
    net_send_message_to_all_players(PTO_C2C_CHAT,js)
end

stub[PTO_C2S_CREATE_PLAYER] = function(req, js)
    local props = cjson.decode(js)
    local pid = props[tostring(PROP_ID)]
    local actor = actor_manager_fetch_player_by_id(pid)
    actor:SetProperties(props)
end



teams = {}
stub[PTO_C2S_MAKE_TEAM] = function(req,js)
    local leader = actor_manager_fetch_player_by_id(req.leader) 
    local member = actor_manager_fetch_player_by_id(req.member) 
    local team = {}
    team.leader = req.leader
    team.id = os.time()
    team.members = { req.leader, req.member }
    table.insert(teams, team)
    -- leader:SetTeam(team.id)
    -- member:SetTeam(team.id)

    net_send_message_to_all_players(PTO_S2C_MAKE_TEAM, cjson.encode(team))
end


