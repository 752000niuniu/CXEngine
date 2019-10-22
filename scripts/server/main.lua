
script_system_dofile('../share/enums.lua')
script_system_dofile('../share/vfs.lua')
script_system_dofile('../share/utils.lua')
script_system_dofile('../share/content_system.lua')
script_system_dofile('../share/actor_metatable.lua')
script_system_dofile('../generator/actor_template.lua')

function on_script_system_init()
    content_system_init()
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

function save_player_database()
    local players = actor_manager_fetch_all_players()
    local pinfos = {}
    for pid, p in pairs(players) do
        local pinfo = {}
        pinfo.pid = p:GetID()
        pinfo.name = p:GetProperty(PROP_NAME)
        pinfo.scene_id = p:GetProperty(PROP_SCENE_ID)
        pinfo.role_id = p:GetProperty(PROP_ROLE_ID)
        pinfo.weapon_id = p:GetProperty(PROP_WEAPON_ID) 
        pinfo.x,pinfo.y = p:GetProperty(PROP_POS)
        table.insert(pinfos, pinfo)
    end
    table.sort(pinfos, function(a,b) return a.pid < b.pid  end)
    	
	local path = vfs_get_workdir() .. '/res/storage/player.data'
	local fw = io.open(path,'w')
    if not fw then return end
    cxlog_info('cjson.encode(pinfos)', cjson.encode(pinfos))
	fw:write(cjson.encode(pinfos))
	fw:close()
end

function game_server_dispatch_message(pt)
    local type = pt:ReadAsInt()
    local js = pt:ReadAllAsString()
    print('game_server_dispatch_message' , type, js)
    local req = cjson.decode(js)
    if  type == PTO_C2C_LOGIN then
        pto_c2s_login(req)
    elseif type == PTO_C2C_LOGOUT then
        
    elseif type == PTO_C2C_MOVE_TO_POS then
        local player = actor_manager_fetch_player_by_id(req.pid)
        player:SetProperty(PROP_POS,req.x,req.y)
        net_send_message_to_all_players(PTO_C2C_MOVE_TO_POS,js)
    elseif type == PTO_C2C_CHAT then
        net_send_message_to_all_players(PTO_C2C_CHAT,js)
    elseif type == PTO_C2C_SAVE_PLAYER_DATABASE then
        save_player_database()
    end
end