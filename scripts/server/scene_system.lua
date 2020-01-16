
local npcs = { }

function scene_system_init()
    local ostime = os.time()
    local npc_table = content_system_get_table('npc')
    local npcs = {}
    for i, props in ipairs(npc_table) do
        local npc = actor_manager_create_actor(utils_next_uid('npc'))
        npc:SetProperties(props)    
        table.insert(npcs, npc)
    end

    local team 
    for i, npc in ipairs(npcs) do
        if i <= 10 then
            if i == 1 then
                team = team_system_create_team(npc)
            else
                team:AddMember(npc)
            end
        end
    end

    for i, npc in ipairs(npcs) do
        if i > 11 then
            if i == 12 then
                team = team_system_create_team(npc)
            else
                team:AddMember(npc)
            end
        end
    end
end

function scene_system_update()

end

function scene_system_stop()
end


stub[PTO_C2S_CLICK_NPC] = function(req)
    local player = actor_manager_fetch_player_by_id(req.pid)
    local target = actor_manager_fetch_player_by_id(req.target)
    if player and target then
        net_send_message(req.pid, PTO_S2C_CLICK_NPC, cjson.encode(req))
    end
end 