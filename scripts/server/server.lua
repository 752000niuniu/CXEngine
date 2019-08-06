
dofile('../share/vfs.lua')
dofile('../share/utils.lua')

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
        for i,v in ipairs(db)
            player_database[v.pid] = v
        end
    end
end


function valid_player_login(msg)
	return true
end

function server_thread_start()
	read_player_database()
end

function server_thread_on_message(conn, buf, netq)
	while buf:readable_size() >= CX_MSG_HEADER_LEN do 
		local len = buf:PeekAsInt()
		if buf:readable_size() >= len + CX_MSG_HEADER_LEN then
			buf:Consume(CX_MSG_HEADER_LEN)
			local type = buf:PeekAsInt()
			if type == PTO_C2S_LOGIN then
				buf:Consume(4)
				local msg = buf:ReadAsString(buf,len-4)
				local pinfo = valid_player_login(msg)
				if pinfo then
					insert_pid_connection_pair(pinfo.pid, conn)
					net_send_message(pinfo.pid, PTO_S2C_LOGIN, pinfo)
				end
			else
				netq:push_back(0,buf:ReadAsString(len))
			end
		else 
			break
		end
	end
end