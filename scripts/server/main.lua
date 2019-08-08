

script_system_dofile('../share/vfs.lua')
script_system_dofile('../share/utils.lua')


local player_database = {}
function read_player_database()
    local path = vfs_makepath('storage/server/player.data') 
    local file = io.open(path,'r')
    if not file then
        local fw = io.open(path,'w')
        fw:close()
        file = io.open(path,'r')
    end
    local data = file:read('a')
    file:close()
    
    local db =  cjson.decode(data)
    if db then
        for i,v in ipairs(db) do
            player_database[v.pid] = v
        end
    end
end

function on_script_system_init()
    -- read_player_database()
    game_server_start(45000)
end

function on_script_system_update()
    game_server_update()
    return true
end


function on_script_system_deinit()
    game_server_stop()
end



function game_server_dispatch_message(pt)
    local type = pt:ReadAsInt()
    local msgjs = pt:ReadAllAsString()
    print('game_server_dispatch_message' , type, msgjs)
    local msg = cjson.decode(msgjs)
    if  type == PTO_C2S_LOGIN then
        -- local player = actor_manager_create_player(msg.pid)
        print('send ', msg.pid, PTO_S2C_PLAYER_ENTER, msgjs)
        net_send_message(msg.pid,PTO_S2C_PLAYER_ENTER,msgjs)
        
    elseif type == PTO_C2S_LOGOUT then
        
    elseif type == PTO_C2S_MOVE_TO_POS then
        net_send_message_to_all_players(PTO_C2S_MOVE_TO_POS, msgjs)
    elseif type == PTO_C2S_CHAT then
        net_send_message_to_all_players(PTO_C2S_CHAT, msgjs)
    end
end