
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

function on_imgui_update()
	if imgui.Button('启动客户端') then
		os.execute(string.format('start %sbin/Debug/SimpleEngine.exe --cwd=%s', vfs_get_workdir(),vfs_get_workdir()))
	end

	if imgui.Button('启动服务器') then
		os.execute(string.format('start %sbin/Debug/SimpleServer.exe --cwd=%s', vfs_get_workdir(),vfs_get_workdir()))
	end
end