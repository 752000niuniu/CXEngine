print('client.lua')

local event_loop = ez_event_loop_create()
local client = ez_tcp_client_create(event_loop, '127.0.0.1',45000,'Launcher')
client:set_on_connection(function(conn)
    print('set_on_connection')
end)

client:set_on_message(function(conn, buf, ts)
    print('set_on_message')
end)

client:Connect()

event_loop:Run()