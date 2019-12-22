script_system_dofile('../share/enums.lua')
script_system_dofile('../share/vfs.lua')
script_system_dofile('../share/utils.lua')


local AccountSB = imgui.CreateStrbuf('oceancx',256)
local PasswordSB = imgui.CreateStrbuf('123456',256)
local IPSB = imgui.CreateStrbuf('127.0.0.1',256)
local PortSB = imgui.CreateStrbuf('45000',256)
local DbgPortSB = imgui.CreateStrbuf('9527',256)
local PlayerNameSB = imgui.CreateStrbuf('Ocean藏心',256)
local PosX = imgui.CreateStrbuf('200',128)
local PosY = imgui.CreateStrbuf('2790',128)

on_script_system_init = function()
	iw_init()
	iw_set_font(vfs_get_workdir()..'/res/font/simsun.ttc')
end

on_script_system_update = function()
	if iw_should_close() then return false end
	iw_begin_render()
	on_imgui_update()
	iw_end_render()
	return true
end

on_script_system_deinit = function()
	iw_deinit()
end

local show_demo = false
function on_imgui_update()
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
	imgui.End()
end