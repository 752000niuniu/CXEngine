--[[
server 连接多个iPhone ， 多个u3d端
iPhone收集到的面捕数据传到server上，server把数据下发到u3d
每个server
]]--

cx_server = nil
CX_MSG_HEADER_LEN = 4
function update()
    -- cxlog_info("dt " .. delta_time())
    local log = ''
    while not shared_netq:empty(0) do
        local pt = shared_netq:front(0)
        local type = pt:ReadAsInt()
        local tm = pt:ReadAsInt64()
        local coefflen = pt:ReadAsInt()
        log = log .. ' type ' .. type .. ' tm ' .. tm .. ' coefflen ' ..
                  coefflen
        for i = 1, coefflen do
            local strlen = pt:ReadAsInt()
            local name = pt:ReadAsString(strlen)
            local val = pt:ReadAsFloat()
            -- cxlog_info(name .. ' : ' .. val)
            log = log .. '\n' .. name .. ':' .. val
        end
        shared_netq:pop_front(0)
        -- local js = pt:ReadAllAsString()
        -- local req = cjson.decode(js)
        -- cxlog_info('read msg ', js)
    end
    cxlog_info(log)
    imgui.Text('dt:' .. delta_time())
end

do
    at_exit_manager_init()
    io_service_context_init()

    local dbg_port = command_arg_opt_int('dbg_port', 9400)
    luadbg_listen(dbg_port)

    event_loop = ez_event_loop_create()
    cx_server = ez_tcp_server_create(event_loop, 2701, 'ARKit')

    shared_netq = net_thread_queue_create()

    cx_server:set_on_connection(function(conn)
        cxlog_info('conn-connected : ', conn:connected())
    end)
    cx_server:set_on_message(function(conn, buf, ts)
        while buf:readable_size() >= CX_MSG_HEADER_LEN do
            local len = buf:PeekAsInt()
            cxlog_info('len : ', len)
            if buf:readable_size() >= len + CX_MSG_HEADER_LEN then
                buf:Consume(CX_MSG_HEADER_LEN)
                shared_netq:push_back(0, buf, len)
                buf:Consume(len)
            else
                break
            end
        end
    end)

    cx_server:Start()
    event_loop:RunTaskEvery(function()
        if iw_should_close() then
            event_loop:Quit()
            return
        end
        iw_begin_render()
        update()
        iw_end_render()
    end, 16)

    iw_init(800, 600)
    iw_set_font(vfs_get_workdir() .. '/res/font/simsun.ttc')

    event_loop:Run()
    iw_deinit()
end
