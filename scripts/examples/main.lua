SCREEN_WIDTH = 800
SCREEN_HEIGHT = 600
DefaultMapID = 1001

SERVER_HOST = command_arg_opt_str('host','127.0.0.1')
SERVER_PORT = command_arg_opt_int('port', 45000)
DBG_PORT = command_arg_opt_int('dbg_port', 9600)
local anim
function init()
    anim = animation_create(23,0xCA8FDEAD)
    anim:SetLoop(0)
    anim:Play()
end
function update()
    -- gl.clear_color(0.67, 0.67, 0.87, 1.0)
    -- gl.clear("color", "depth")
    -- if anim then
    --     anim:Update()
    --     anim:Draw()
    -- end
end


do
	at_exit_manager_init()
    io_service_context_init()
    luadbg_listen(DBG_PORT)
    iw_init(SCREEN_WIDTH,SCREEN_HEIGHT)
    iw_set_font(vfs_get_workdir()..'/res/font/msyhl.ttc')
    init()
    iw_render(update)
    iw_deinit()
end