script_system_dofile('../share/enums.lua')
script_system_dofile('../share/vfs.lua')
script_system_dofile('../share/utils.lua')
script_system_dofile('../share/enums_protocol.lua')
CX_MSG_HEADER_LEN = 4 

local event_loop  
local cx_client 
local shared_netq

local AccountSB = imgui.CreateStrbuf('oceancx',256)
local PasswordSB = imgui.CreateStrbuf('123456',256)
local IPSB = imgui.CreateStrbuf('127.0.0.1',256)
local PortSB = imgui.CreateStrbuf('45000',256)
local DbgPortSB = imgui.CreateStrbuf('9527',256)
local PlayerNameSB = imgui.CreateStrbuf('Ocean藏心',256)
local PosX = imgui.CreateStrbuf('200',128)
local PosY = imgui.CreateStrbuf('2790',128)


function net_send_message(pt, msg)
	cxlog_info('net_send_message',pt ,msg)
	local buf = ezio_buffer_create()
	buf:WriteInt(pt)
	buf:WriteString(msg)
	local cnt = buf:readable_size()
	buf:PrependInt(cnt)
	shared_netq:push_back(1,buf,buf:readable_size())
	ezio_buffer_destroy(buf)
end


local show_demo = false
function on_imgui_update()
	if not shared_netq:empty(0) then
		local pt = shared_netq:front(0)
		local type = pt:ReadAsInt()
		local js = pt:ReadAllAsString()
		local req = cjson.decode(js)
		cxlog_info('read msg ', js)

		shared_netq:pop_front(0)
	end

	if cx_client:IsConnected() then 
		while not shared_netq:empty(1) do
			local req = shared_netq:front(1)
			local conn = cx_client:connection()
			conn:Send(req)
			shared_netq:pop_front(1)
		end
	end

	imgui.Begin('NewWindow')
	

	if imgui.Button('Reload') then
		script_system_dofile('main.lua')
	end

	res,show_demo = imgui.Checkbox('Demo',show_demo)
	if show_demo then
		imgui.ShowDemoWindow()
	end

	imgui.Text('IP  :')
	imgui.SameLine()
	imgui.InputText('##IP', IPSB)

	imgui.Text('Port  :')
	imgui.SameLine()
	imgui.InputText('##Port', PortSB)

	imgui.Text('DbgPort  :')
	imgui.SameLine()
	imgui.InputText('##DbgPort', DbgPortSB)

	imgui.Text("Account   :");
	imgui.SameLine();
	imgui.InputText("##account", AccountSB);

	imgui.Text("Password   :");
	imgui.SameLine();
	imgui.InputText("##password", PasswordSB);

	
	if imgui.Button('启动客户端') then
		local exepath = vfs_get_workdir()..'bin/Debug/SimpleEngine.exe'
		local tcmd = {
			'start '..exepath,
			'--cwd='..vfs_get_workdir(),
			'--host='..IPSB:str(),
			'--port='..PortSB:str(),
			'--dbg_port='..DbgPortSB:str(),
			'--user='..AccountSB:str(),
			'--pass='..PasswordSB:str(),
		}
		local cmd = table.concat(tcmd,' ')
		cxlog_info(cmd)
		os.execute(cmd)
	end

	if imgui.Button('启动客户端2') then
		local exepath = vfs_get_workdir()..'bin/Debug/SimpleEngine.exe'
		local dbg_port = math.tointeger(DbgPortSB:str())

		local tcmd = {
			'start '..exepath,
			'--cwd='..vfs_get_workdir(),
			'--host='..IPSB:str(),
			'--port='..PortSB:str(),
			'--dbg_port='..(dbg_port+10),
			'--user='..AccountSB:str()..'2',
			'--pass='..PasswordSB:str(),
		}
		local cmd = table.concat(tcmd,' ')
		cxlog_info(cmd)
		os.execute(cmd)
	end

	if imgui.Button('启动服务器') then
		local cmd = string.format('start %sbin/Debug/SimpleServer.exe --cwd=%s', vfs_get_workdir(),vfs_get_workdir())
		cxlog_info(cmd)
		os.execute(cmd)
	end

	if imgui.Button('启动DA 4711') then
		local path = vfs_makepath('internals/luadebugger/vscode/Debug/vsdbgadapter.exe')
		local cwd = vfs_makepath('internals/luadebugger/vscode/')
		local cmd = string.format('start %s --cwd=%s --port=4711',path,cwd)
		os.execute(cmd)
	end
	if imgui.Button('启动DA 4712') then
		local path = vfs_makepath('internals/luadebugger/vscode/Debug/vsdbgadapter.exe')
		local cwd = vfs_makepath('internals/luadebugger/vscode/')
		local cmd = string.format('start %s --cwd=%s --port=4712',path,cwd)
		os.execute(cmd)
	end
	if imgui.Button('生成imguiBinding') then
		script_system_dofile('../generator/imgui_binding_generator.lua')
	end

	if imgui.Button('生成luadbg.inl') then
		local debuggee_path = vfs_makepath('internals/luadebugger/luadbg/debuggee.lua') 
		local debugger_path = vfs_makepath('internals/luadebugger/luadbg/debugger.lua') 
		local luadbg_inl_path = vfs_makepath('internals/luadebugger/luadbg/src/luadbg.inl') 
		local luadbg_inl_file = io.open(luadbg_inl_path,'w+')
		luadbg_inl_file:write([[const char* debuggee_code = "--__debuggee_code__"]]..'\n')
		local lines = {}
		for line in io.lines(debuggee_path) do
			table.insert(lines, string.format('%q',line)..'"\\n"')
		end
		luadbg_inl_file:write(table.concat(lines,'\n')..';\n')
		luadbg_inl_file:write('\n')

		luadbg_inl_file:write([[const char* debugger_code = "--__debugger_code__"]]..'\n')
		lines = {}
		for line in io.lines(debugger_path) do
			table.insert(lines, string.format('%q',line)..'"\\n"')
		end
		luadbg_inl_file:write(table.concat(lines,'\n')..';\n')
		luadbg_inl_file:write('\n')
		luadbg_inl_file:close()
	end

	if imgui.Button('重新生成ActorProp') then
		script_system_dofile('../generator/actor_template.lua')
	end

	if imgui.Button('table_template') then
		-- script_system_dofile('generator/table_template.lua')
		-- generate_avatar_role_tsv()
	end
	if imgui.Button('安装VSCode插件') then
		local dir = vfs_makepath('')
		local cmd = string.format('sh  %sinternals/luadebugger/install_extension.sh %s',dir,dir)
		print(cmd)
		os.execute(cmd)
	end
	-- if imgui.Button('生成Protocol') then
	-- 	script_system_dofile('../generator/protocol.lua')	
	-- end
	if imgui.Button('登陆') then
		local msg = {}
		msg.account = 'oceancx'
		msg.password = '123456'
		net_send_message(PTO_C2C_LOGIN, cjson.encode(msg))
	end
	imgui.End()
end

do
	at_exit_manager_init()
	io_service_context_init()
	luadbg_listen(9529)
	luadbg_enable_log(true)

	iw_init()
	iw_set_font(vfs_get_workdir()..'/res/font/simsun.ttc')

	event_loop = ez_event_loop_create()
	cx_client = ez_tcp_client_create(event_loop, '127.0.0.1',45000,'Launcher')
	shared_netq = net_thread_queue_create()
	cx_client:set_on_connection(function(conn)
		-- cxlog_info('conn-connected : ', conn:connected())
	end)

	cx_client:set_on_message(function(conn, buf, ts)
		while buf:readable_size() >= CX_MSG_HEADER_LEN do
			local len = buf:PeekAsInt()
			if buf:readable_size() >= len + CX_MSG_HEADER_LEN then
				buf:Consume(CX_MSG_HEADER_LEN)
				shared_netq:push_back(0, buf, buf:readable_size())
				buf:Consume(len)
			else
				break
			end
		end
	end)

	cx_client:Connect()
	
	event_loop:RunTaskEvery(function()
		if iw_should_close() then 
			event_loop:Quit()
			return
		end
		iw_begin_render()
		on_imgui_update()
		iw_end_render()
	end,16)
	event_loop:Run()
	iw_deinit()
end
