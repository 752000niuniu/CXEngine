

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
    print('game_server_dispatch_message' , pt:Preview(pt:readable_size()))
    local type = pt:ReadAsInt()
    if  type == PTO_C2S_LOGIN then
        --[[
            登录协议, 接收到消息 , 读数据库里的数据, 对登陆信息进行验证,
            验证成功后 服务器创建player 
            同步给所有客户端 某某玩家登陆
            某某玩家收到后, 本地创建玩家 设置成local 
            其他玩家收到后, 也创建玩家 不设置local 
            同时同步所有玩家到某某玩家
        ]]
        local msgjs =  pt:ReadAllAsString()
        local msg = cjson.decode(msgjs)

        net_send_message(msg.pid,PTO_S2C_PLAYER_ENTER,msgjs)

    elseif type == PTO_C2S_LOGOUT then
        -- local msg = __parse_pt(pt)
    elseif type == PTO_C2S_MOVE_TO_POS then
        -- local msg = __parse_pt(pt)
    elseif type == PTO_C2S_CHAT then
        -- local msg = __parse_pt(pt)/
        

    end
end