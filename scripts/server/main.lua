

stub = {}

script_system_dofile('../share/enums.lua')
script_system_dofile('../share/vfs.lua')
script_system_dofile('../share/utils.lua')
script_system_dofile('../share/content_system.lua')
script_system_dofile('../share/actor_metatable.lua')
-- script_system_dofile('../generator/actor_template.lua')

script_system_dofile('login_system.lua')
script_system_dofile('actor_system.lua')
script_system_dofile('../combat/server/combat_system.lua')

function server_reload()
    cxlog_info('server_reload   ')
    script_system_dofile('main.lua')
end

function on_script_system_init()
    content_system_init()
    game_server_start(45000)
end

function on_script_system_update()
    game_server_update() --dispatch message
    combat_system_update_battle()

    
    local players = actor_manager_fetch_all_players()
    local dirty_props = {}
    for pid, p in pairs(players) do
        if p:IsDirty() then
            dirty_props[pid] = {}
            local props = p:GetDirtyProps()
            for i,pidx in ipairs(props) do
                dirty_props[pid][pidx] = p:GetProperty(pidx)
            end
            cxlog_info('dirty pros ', cjson.encode(props))
            dirty_props[pid] = props
        end
    end

    for pid, p in pairs(players) do
        if p:IsDirty() then
            net_send_message(pid,PTO_S2C_SYNC_PROPS, cjson.encode(dirty_props))
            p:ClearDirty()
        end
    end
    
    return true
end


function on_script_system_deinit()
    game_server_stop()
end

stub[PTO_C2S_DOSTRING] = function(req)
    local func, err = load(req.code,'@client','bt',_ENV)
    if func then
        func()
    else
        cxlog_info('PTO_C2S_DOSTRING', err)
    end
end 

stub[PTO_C2C_SAVE_PLAYER_DATABASE] = function()
    local players = actor_manager_fetch_all_players()
    local pinfos = {}
    for pid, p in pairs(players) do
        local pinfo = {}
        pinfo.pid = p:GetID()
        pinfo.name = p:GetProperty(PROP_NAME)
        pinfo.scene_id = p:GetProperty(PROP_SCENE_ID)
        pinfo.role_id = p:GetProperty(PROP_ROLE_ID)
        pinfo.weapon_id = p:GetProperty(PROP_WEAPON_ID) 
        pinfo.x,pinfo.y = p:GetPos()
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
    if stub[type] then
        stub[type](req,js)
    end
end

