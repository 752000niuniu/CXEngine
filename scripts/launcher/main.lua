
on_script_system_init = function()
	iw_init()
	iw_set_font(vfs_get_workdir()..'/res/font/simsun.ttc')
end

on_script_system_update = function()
	if iw_should_close() then return false end
	iw_begin_render()

	imgui.Button('hllo')
	iw_end_render()
	return true
end

on_script_system_deinit = function()
	iw_deinit()
end