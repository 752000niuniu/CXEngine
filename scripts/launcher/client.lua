print('client.lua')
CX_MSG_HEADER_LEN = 4 

local event_loop = ez_event_loop_create()
local client = ez_tcp_client_create(event_loop, '127.0.0.1',45000,'Launcher')
client:set_on_connection(function(conn)
    -- local buf = ezio_buffer_create()
    -- buf:WriteString(cjson.encode({ type='debuggee', event='connection_state', connected = conn:connected(), addr = conn:tohostport()  }))
    -- shared_netq:push_back(0,buf,buf:readable_size())
    -- ezio_buffer_destroy(buf)
end)

client:set_on_message(function(conn, buf, ts)
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

client:Connect()

event_loop:RunTaskEvery(function()
    if not client:IsConnected() then return end
    while not shared_netq:empty(1) do
        local req = shared_netq:front(1)
        local conn = client:connection()
        conn:Send(req)
        shared_netq:pop_front(1)
    end
end,10)

event_loop:Run()