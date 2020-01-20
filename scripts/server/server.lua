script_system_dofile('../share/vfs.lua')
script_system_dofile('../share/utils.lua')



local player_database = {}
function read_player_database()
	local path = vfs_get_workdir() .. '/res/storage/player.data'
	local db = read_database_file(path)
	if db then
		for i,v in ipairs(db) do
			local pid = v[tostring(PROP_ID)]
			player_database[pid] = v
		end
	end
end

stub[PTO_C2C_SAVE_PLAYER_DATABASE] = function()
    local players = actor_manager_fetch_all_actors()
	local pinfos = {}
    for pid, p in pairs(players) do
        if p:GetProperty(PROP_ACTOR_TYPE) == ACTOR_TYPE_PLAYER then
			table.insert(pinfos, p:GetProperties())
        end
    end
    table.sort(pinfos, function(a,b) return a[PROP_ID] < b[PROP_ID] end)
    	
	local path = vfs_get_workdir() .. '/res/storage/player.data'
	local fw = io.open(path,'w')
    if not fw then return end
	fw:write(cjson.encode(pinfos))
	fw:close()
end

function fetch_player_database_props(pid)
 	return player_database[pid]
end

local account_database = {}
function read_account_database()
	local path = vfs_get_workdir() .. '/res/storage/account.data'
	local db = read_database_file(path)
	if db then
		for i,v in ipairs(db) do
			account_database[v.account] = v
		end
	end
end

stub[PTO_C2C_SAVE_ACCOUNT_DATABASE] = function()
	local accounts = {}
	for account,info in pairs(account_database) do
		table.insert(accounts, info)
	end
	table.sort(accounts,function(a,b) return a.pid < b.pid end)

    local path = vfs_get_workdir() .. '/res/storage/account.data'
	local fw = io.open(path,'w')
    if not fw then return end
	fw:write(cjson.encode(accounts))
	fw:close()
end

function server_on_disconnect(pid)
	cxlog_info('server_on_disconnect',pid)
	combat_system_battle_on_actor_leave(pid)
end

local idincr = 1
function server_thread_on_message(conn, buf, netq)
	while buf:readable_size() >= CX_MSG_HEADER_LEN do 
		local len = buf:PeekAsInt()
		if buf:readable_size() >= len + CX_MSG_HEADER_LEN then
			buf:Consume(CX_MSG_HEADER_LEN)
			local type = buf:PeekAsInt()
			if type == PTO_C2C_SIGNUP then
				buf:Consume(4)
				local msgjs = buf:ReadAsString(len-4)
				local msg = cjson.decode(msgjs)
				print('PTO_C2C_SIGNUP', msgjs)
				local account_info = {}
				account_info.pid = math.tointeger(os.time() + idincr)
				idincr = idincr + 1
				account_info.account = msg.account
				account_info.password = msg.password
				account_database[account_info.account] = account_info
				print('account_info ' , cjson.encode(account_info))
			elseif type == PTO_C2C_LOGIN then
				buf:Consume(4)
				local msgjs = buf:ReadAsString(len-4)
				print('PTO_C2C_LOGIN', msgjs)
				local msg = cjson.decode(msgjs)
				local info = account_database[msg.account]
				if not info then 
					cxlog_info('account not exist!')
					return 
				end
				if info.password ~= msg.password then
					cxlog_info('password wrong!')
					return 
				end

				if info.pid then
					local pid = info.pid
					print('pid', pid)
					erase_pid_connection_pair(pid)
					insert_pid_connection_pair(pid, conn)
					msg.pid = pid
					local newmsg = ezio_buffer_create()
					newmsg:WriteInt(PTO_C2C_LOGIN)
					newmsg:WriteString(cjson.encode(msg))
					print('newmsg', cjson.encode(msg))
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