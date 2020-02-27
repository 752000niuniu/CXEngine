script_system_dofile('../generator/sound_tsv_parser.lua')
script_system_dofile('summon_handle.lua')

local AccountSB = imgui.CreateStrbuf('simulator',256)
local PasswordSB = imgui.CreateStrbuf('123456',256)
local SourceSB = imgui.CreateStrbuf('print("hello")',2560)
local IPSB = imgui.CreateStrbuf('127.0.0.1',256)
local PortSB = imgui.CreateStrbuf('45000',256)
local DbgPortSB = imgui.CreateStrbuf('9600',256)
local DataPathSB = imgui.CreateStrbuf(vfs_get_workdir()..'data/' ,256)
local PlayerNameSB = imgui.CreateStrbuf('simulator',256)
local PosX = imgui.CreateStrbuf('200',128)
local PosY = imgui.CreateStrbuf('2790',128)

local account_infos = {}

stub = stub or {}
stub[PTO_S2C_GM] = function(req)
	cxlog_info('PTO_S2C_GM', cjson.encode(req))
	if req then
		account_infos = req
	end
end
stub[PTO_C2C_PLAYER_ENTER] = function()
	
end

function net_manager_stub()
	return stub
end
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
		if stub[type] then
			stub[type](req)
		end
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

	local viewport, x,y,w,h = imgui.GetMainViewport();
	imgui.SetNextWindowPos(x,y);
	imgui.SetNextWindowSize(w,h);
	imgui.SetNextWindowViewport(viewport);


	imgui.Begin('NewWindow', nil, ImGuiWindowFlags_NoDecoration)
	if imgui.Button('Reload') then
		script_system_dofile('window.lua')
	end

	imgui.Button('启动游戏说明')
	if imgui.IsItemHovered() then
		imgui.BeginTooltip()
		imgui.Text([[先点"启动服务器"，然后点"注册帐号"，
注册完之后，点"更新帐号信息"，
下面会根据帐号信息刷新出来新的按钮，
点击这些按钮就是启动对应帐号的客户端。
如果刷新不出来，帐号信息刷新按钮可以多点几次
帐号信息保存在res/storage/account.data

梦幻西游经典版目录下的所有资源放到data/目录下

有问题请加群：983630090]])
		imgui.EndTooltip()
	end


	if command_arg_check('Debug') then

		res,show_demo = imgui.Checkbox('Demo',show_demo)
		if show_demo then
			imgui.ShowDemoWindow()
		end
 

		imgui.Text('Server:')
		imgui.InputTextMultiline('##source', SourceSB, 300, 50,ImGuiInputTextFlags_AllowTabInput)
	   
		if imgui.Button('服务端执行') then
			local code = SourceSB:str()
			net_manager_player_dostring(code)
		end
		imgui.SameLine()
		
	end

	
	if imgui.Button('启动服务器') then
		local exepath
		if command_arg_check('Debug') then
			exepath = vfs_get_workdir()..'bin/Debug/SimpleServer.exe'
		else
			exepath = vfs_get_workdir()..'bin/SimpleServer.exe'
		end

		local cmd = string.format('start %s --cwd=%s', exepath,vfs_get_workdir())
		cxlog_info(cmd)
		os.execute(cmd)
	end
	imgui.SameLine()
	if imgui.Button('重连服务器') then
		net_reconnect()
	end

	imgui.SameLine()
	if imgui.Button('关闭服务器') then
	end
	imgui.SameLine()
	if imgui.Button('关闭所有端') then
		net_send_message(PTO_C2S_GM, cjson.encode({type='exit'}))
		net_send_message(PTO_C2S_GM, cjson.encode({type='exit_server'}))
	end
	imgui.SameLine()
	if imgui.Button('关闭客户端') then
		net_send_message(PTO_C2S_GM, cjson.encode({type='exit'}))
	end

	if command_arg_check('Debug') then
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

		if imgui.Button('Test') then

			local tbl,cnames = utils_parse_tsv('battle_map')
			local tbl2,cnames= utils_parse_tsv('avatar_summon')

			local names1={}
			for i,row in ipairs(tbl) do
				local sps = utils_string_split(row.monster,'、') 
				for i,k in ipairs(sps) do
					names1[k] = true
				end
			end

			local names2={}
			for i,row in ipairs(tbl2) do
				names2[row.ID] = true
			end

			local remain={}
			for k,v in pairs(names1) do
				if not names2[k] then
					table.insert(remain,k)
				end
			end
			local s=1;
		end

		if imgui.Button('HandleAvatarNpc') then
			handle_avatar_npc()
		end

		if imgui.Button('TestHandle') then
			output_names()
		end
	end

	if imgui.Button('更新帐号信息') then
		net_send_message(PTO_C2S_GM, cjson.encode({type='account_info'}))
		
	end
	
	if imgui.Button('注册帐号') then
		imgui.OpenPopup('RigsterAccount')
	end
	if imgui.BeginPopup('RigsterAccount') then
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

		
		if imgui.Button('注册') then
			local msg = {}
			msg.account = AccountSB:str()
			msg.password = PasswordSB:str()
			net_send_message(PTO_C2C_SIGNUP, cjson.encode(msg))
			imgui.CloseCurrentPopup()
		end

		imgui.SameLine()

	
		imgui.EndPopup('RigsterAccount')
	end

	if next(account_infos) then
		imgui.HorizontalLayout(account_infos,next,function(k,v)
			if imgui.Button(string.format('%s:%d', v.account, v.pid).."##"..k) then
                local exepath
				if command_arg_check('Debug') then
					exepath = vfs_get_workdir()..'bin/Debug/SimpleEngine.exe'
				else
					exepath = vfs_get_workdir()..'bin/SimpleEngine.exe'
				end
				local tcmd = {
					'start '..exepath,
					'--cwd='..vfs_get_workdir(),
					'--host='..IPSB:str(),
					'--port='..PortSB:str(),
					'--dbg_port='..DbgPortSB:str(),
					'--user='..v.account,
					'--pass='..v.password,
				}
				local cmd = table.concat(tcmd,' ')
				cxlog_info(cmd)
				os.execute(cmd)
            end
		end)
	end
	imgui.End()
end


