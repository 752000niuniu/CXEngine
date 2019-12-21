

stub = {}

luadbg_listen(9528)

script_system_dofile('../share/enums.lua')
script_system_dofile('../share/enums_protocol.lua')
script_system_dofile('../share/vfs.lua')
script_system_dofile('../share/utils.lua')
script_system_dofile('../share/content_system.lua')
script_system_dofile('../share/actor_metatable.lua')

script_system_dofile('server.lua')
script_system_dofile('login_system.lua')
script_system_dofile('actor_system.lua')
script_system_dofile('scene_system.lua')
script_system_dofile('../combat/combat_system.lua')

function server_reload()
    cxlog_info('server_reload')
    script_system_dofile('main.lua')
    content_system_init()
    init_skills()
    init_buffers()
end

function on_script_system_init()
    content_system_init()
    init_skills()
    init_buffers()
    scene_system_init()
end

local prop_templ_tbl
function is_prop_sync(prop_id)
    if not prop_templ_tbl then
        prop_templ_tbl = content_system_get_table('actor_template')
    end
    return prop_templ_tbl[prop_id+1].sync ~= 0
end

function on_script_system_update()
    game_server_update()  
    combat_system_update_battle()
    scene_system_update()
    
    local players = actor_manager_fetch_all_players()
    local dirty_props = {}
    
    for _, p in ipairs(players) do
        if p:IsDirty() then
            local pid = p:GetID()
            local props = p:GetDirtyProps()
            for i,prop_id in ipairs(props) do
                if is_prop_sync(prop_id) then
                    table.insert(dirty_props, {pid, prop_id, p:GetProperty(prop_id)})
                end
            end
            p:ClearDirty()
        end
    end
    if #dirty_props > 0 then
        cxlog_info('sync dirty props', #dirty_props) 
        net_send_message_to_all_players(PTO_S2C_SYNC_PROPS, cjson.encode(dirty_props))
    end
        
    return true
end


function on_script_system_deinit()
    scene_system_stop()
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



function game_server_dispatch_message(pt)
    local type = pt:ReadAsInt()
    local js = pt:ReadAllAsString()
    print('game_server_dispatch_message' , type, js)
    local req = cjson.decode(js)
    if stub[type] then
        stub[type](req,js)
    end
end

