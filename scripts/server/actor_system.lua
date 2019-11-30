stub[PTO_C2C_MOVE_TO_POS] = function(req,js)
    local player = actor_manager_fetch_player_by_id(req.pid)
    player:SetPos(req.x,req.y)
    net_send_message_to_all_players(PTO_C2C_MOVE_TO_POS,js)
end


stub[PTO_C2C_CHAT] = function(req,js)
    net_send_message_to_all_players(PTO_C2C_CHAT,js)
end

