local AccountSB = imgui.CreateStrbuf('oceancx',256)
local PasswordSB = imgui.CreateStrbuf('123456',256)
local SourceSB = imgui.CreateStrbuf('print("hello")',2560)
local IPSB = imgui.CreateStrbuf('127.0.0.1',256)
local PortSB = imgui.CreateStrbuf('45000',256)
local DbgPortSB = imgui.CreateStrbuf('9600',256)
local PlayerNameSB = imgui.CreateStrbuf('Ocean藏心',256)
local PosX = imgui.CreateStrbuf('200',128)
local PosY = imgui.CreateStrbuf('2790',128)

function net_manager_player_dostring(pid, code)
	local req = {
		pid = pid,
		code = code
	}
	net_send_message(PTO_C2S_PLAYER_DOSTRING,cjson.encode(req))
end

function launcher_init()
	if command_arg_check('gen-imgui') then
		script_system_dofile('../generator/imgui_binding_generator.lua')
		return	
	end
end

function launcher_update()
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
		script_system_dofile('window.lua')
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

	imgui.Text('Server:')
	imgui.InputTextMultiline('##source', SourceSB, 300, 50,ImGuiInputTextFlags_AllowTabInput)
   
	if imgui.Button('服务端执行') then
		local code = SourceSB:str()
		net_manager_player_dostring(code)
	end
	imgui.SameLine()
	if imgui.Button('重连服务器') then
		net_reconnect()
	end

	if imgui.Button('客户端1') then
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

	imgui.SameLine()
	if imgui.Button('客户端2') then
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

	imgui.SameLine()
	if imgui.Button('客户端3') then
		local exepath = vfs_get_workdir()..'bin/Debug/SimpleEngine.exe'
		local dbg_port = math.tointeger(DbgPortSB:str())

		local tcmd = {
			'start '..exepath,
			'--cwd='..vfs_get_workdir(),
			'--host='..IPSB:str(),
			'--port='..PortSB:str(),
			'--dbg_port='..(dbg_port+20),
			'--user='..AccountSB:str()..'3',
			'--pass='..PasswordSB:str(),
		}
		local cmd = table.concat(tcmd,' ')
		cxlog_info(cmd)
		os.execute(cmd)
	end

	imgui.SameLine()
	if imgui.Button('客户端4') then
		local exepath = vfs_get_workdir()..'bin/Debug/SimpleEngine.exe'
		local dbg_port = math.tointeger(DbgPortSB:str())

		local tcmd = {
			'start '..exepath,
			'--cwd='..vfs_get_workdir(),
			'--host='..IPSB:str(),
			'--port='..PortSB:str(),
			'--dbg_port='..(dbg_port+30),
			'--user='..AccountSB:str()..'4',
			'--pass='..PasswordSB:str(),
		}
		local cmd = table.concat(tcmd,' ')
		cxlog_info(cmd)
		os.execute(cmd)
	end

	imgui.SameLine()
	if imgui.Button('客户端5') then
		local exepath = vfs_get_workdir()..'bin/Debug/SimpleEngine.exe'
		local dbg_port = math.tointeger(DbgPortSB:str())

		local tcmd = {
			'start '..exepath,
			'--cwd='..vfs_get_workdir(),
			'--host='..IPSB:str(),
			'--port='..PortSB:str(),
			'--dbg_port='..(dbg_port+40),
			'--user='..AccountSB:str()..'5',
			'--pass='..PasswordSB:str(),
		}
		local cmd = table.concat(tcmd,' ')
		cxlog_info(cmd)
		os.execute(cmd)
	end
	imgui.SameLine()
	if imgui.Button('关闭客户端') then
		net_send_message(PTO_C2S_GM, cjson.encode({type='exit'}))
	end

	if imgui.Button('启动服务器') then
		local cmd = string.format('start %sbin/Debug/SimpleServer.exe --cwd=%s', vfs_get_workdir(),vfs_get_workdir())
		cxlog_info(cmd)
		os.execute(cmd)
	end
	imgui.SameLine()
	if imgui.Button('关闭服务器') then
		net_send_message(PTO_C2S_GM, cjson.encode({type='exit_server'}))
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

	if imgui.Button('SignUp') then
		local msg = {}
        msg.account = AccountSB:str()
        msg.password = PasswordSB:str()
        net_send_message(PTO_C2C_SIGNUP, cjson.encode(msg))
	end

	if imgui.Button('SignIn') then
		local msg = {}
        msg.account = AccountSB:str()
        msg.password = PasswordSB:str()
        net_send_message(PTO_C2C_LOGIN, cjson.encode(msg))
	end

	
	if imgui.Button('战斗') then
		local msg = {}
		msg.atk = 1576985785
		msg.def =1576942487
		net_send_message(PTO_C2S_COMBAT_START, cjson.encode(msg))

	end
	
	if imgui.Button('战斗命令1') then

		local msg = {}
		msg.master = 1576985785
		msg.target = 1576942487  
		msg.skill_id = 1
		net_send_message(PTO_C2S_COMBAT_CMD, cjson.encode(msg) )
	end

	if imgui.Button('战斗命令2') then
		local msg = {}
		msg.master = 1576942487 
		msg.target = 1576985785
		msg.skill_id = 1
		net_send_message(PTO_C2S_COMBAT_CMD, cjson.encode(msg) )
	end

	imgui.End()
end
