

local anim 
function init()
    sprite_renderer_init()
    timer_manager_init()
    anim =  animation_create(23,0xCA8FDEAD)
    anim:SetPos(100,100)
    anim:SetLoop(0)
end

function update()
    -- gl.viewport(0, 0, 800, 600)
    -- gl.clear_color(1.0, 0, 0.3, 1.0) 
    -- gl.clear("color", "depth",'stencil');

    timer_manager_update(16)
    if anim then
        anim:Update()
    end
end

function draw()
    if anim then
        anim:Draw()
    end
end

function draw_imgui()
    if imgui.Button('wht') then
    end
end

do
	at_exit_manager_init()
	io_service_context_init()

	iw_init()
	iw_set_font(vfs_get_workdir()..'/res/font/msyhl.ttc')
	event_loop = ez_event_loop_create()
    init()
	event_loop:RunTaskEvery(function()
		if iw_should_close() then 
			event_loop:Quit()
			timer_manager_deinit()
			return
        end
        update()
        iw_render(draw,draw_imgui)

	end,16)
	event_loop:Run()
	iw_deinit()
end
