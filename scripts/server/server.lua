script_system_dofile('../share/vfs.lua')
script_system_dofile('../share/utils.lua')

local player_database = {}
function read_player_database()
    local path = vfs_get_workdir() .. '/storage/server/player.data'
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

function server_thread_start()
	read_player_database()
end

local idincr= 1
function server_thread_on_message(conn, buf, netq)
	while buf:readable_size() >= CX_MSG_HEADER_LEN do 
		local len = buf:PeekAsInt()
		if buf:readable_size() >= len + CX_MSG_HEADER_LEN then
			buf:Consume(CX_MSG_HEADER_LEN)
			local type = buf:PeekAsInt()
			if type == PTO_C2S_SIGNUP then
				buf:Consume(4)
				local msgjs = buf:ReadAsString(len-4)
				local msg = cjson.decode(msgjs)
				print('PTO_C2S_SIGNUP', msg)
				local pinfo = {}
				pinfo.pid = os.time() + idincr
				idincr = idincr + 1
				pinfo.account = msg.account
				pinfo.password = msg.password
				player_database[pinfo.pid] = pinfo
			elseif type == PTO_C2S_LOGIN then
				buf:Consume(4)
				local msgjs = buf:ReadAsString(len-4)
				print('PTO_C2S_LOGIN', msgjs)
				local msg = cjson.decode(msgjs)
				local pid 
				for _pid, pinfo in pairs(player_database) do
					if pinfo.account == msg.account and pinfo.password == msg.password then
						pid = _pid
						break
					end
				end
				if pid then
					print('pid', pid)
					erase_pid_connection_pair(pid)
					insert_pid_connection_pair(pid, conn)
					msg.pid = pid
					local newmsg = ezio_buffer_create()
					newmsg:WriteInt(PTO_C2S_LOGIN)
					newmsg:WriteString(cjson.encode(msg))
					netq:push_back(0, newmsg,newmsg:readable_size())
					ezio_buffer_destroy(newmsg)					
				end
			else
				netq:push_back(0, buf,len)
				buf:Consume(len)
			end
		else 
			break
		end
	end
end