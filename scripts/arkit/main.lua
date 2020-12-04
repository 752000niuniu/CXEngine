--[[
server 连接多个iPhone ， 多个u3d端
iPhone收集到的面捕数据传到server上，server把数据下发到u3d
每个server 
每个ARKitStreamer把面捕数据传到server 协议 iPhoneToServer
每个登陆到ARKitServer的客户端从ARKitServer里面取最新的帧
]] --
script_system_dofile('../arkit/protocol.lua')
script_system_dofile('../share/utils.lua')
script_system_dofile('enum.lua')
cx_server = nil
event_loop = nil
local connections = {}
local send_to_u3d = false

function net_send_jsonobj(conn, type, obj)
    local js = cjson.encode(obj)
    cxlog_info('net_send_jsonobj ' .. js)
    local buf = ezio_buffer_create()
    buf:WriteInt(PROTO_JSON)
    buf:WriteInt(type)
    buf:WriteInt(js:len())
    buf:WriteString(js)

    local sz = buf:readable_size()
    buf:PrependInt(sz)

    conn:Send(buf)
    ezio_buffer_destroy(buf)
end

function net_send_proto(conn, proto)
    local buf = ezio_buffer_create()
    buf:WriteInt(proto)
    local sz = buf:readable_size()
    buf:PrependInt(sz)
    conn:Send(buf)
    ezio_buffer_destroy(buf)
end

local u3d_infos = {}
local u3d_address = {}
local phone_infos = {}
local phone_address = {}

function send_phone_info_to_u3d()
    local msg = { }
    local infos = {}
    for id, v in pairs(phone_infos) do
        local phone = {}
        phone.accountID = v.accountID
        phone.id = v.id
        phone.ip = v.ip
        phone.port = v.port
        table.insert(infos, phone)
    end
    if #infos == 0 then return end
    msg.infos = infos

    for k, v in pairs(u3d_infos) do
        if v.conn and v.conn:connected() then
            net_send_jsonobj(v.conn, PT_TYPE_ARKIT_IPHONE_REG_OK, msg)
        end
    end
end

function on_iphone_reg(js, conn)
    local info = cjson.decode(js)

    if phone_address[info.ip] == nil then
        phone_address[info.ip] = utils_next_uid('phone')
    end
    local id = phone_address[info.ip]
    info.id = id
    info.conn = conn

    phone_infos[info.id] = info

    local msg = {}
    msg.id = info.id
    net_send_jsonobj(conn, PT_TYPE_ARKIT_IPHONE_REG_OK, msg)

    send_phone_info_to_u3d()
end

function on_u3d_reg(js, conn)
    local info = cjson.decode(js)
    if u3d_address[info.ip] == nil then
        u3d_address[info.ip] = utils_next_uid('u3d')
    end

    local id = u3d_address[info.ip]
    info.id = id
    info.conn = conn
     
    u3d_infos[info.id] = info

    local msg = {}
    msg.id = info.id
    net_send_jsonobj(conn, PT_TYPE_ARKIT_U3D_REG_OK, msg)

    send_phone_info_to_u3d()
end

function ui_update()
    if iw_should_close() then
        event_loop:Quit()
        return
    end
    iw_begin_render()
    imgui.Text('dt:' .. delta_time())
    iw_end_render()
end

--[[
    收到phone的faceframe之后就发送给u3d
    把收到的faceframe 按此frame的发送时间排序
    每10ms向u3d广播一帧
    发送端发送时间不能提前
    接收端可以通过丢弃接收数据 往前追赶
]] --
function on_face_frame(buf, conn, len)
    local sendbuf = ezio_buffer_create()
    sendbuf:WriteInt(PROTO_BYTES)
    sendbuf:WriteInt(PT_TYPE_ARKIT_U3D_FACE_FRAME)
    sendbuf:WriteBuffer(buf, len)
    buf:Consume(len)

    local sz = sendbuf:readable_size()
    sendbuf:PrependInt(sz)
    for u_id, u_info in pairs(u3d_infos) do
        local conn = u_info.conn
        if conn and conn:connected() then conn:Send(sendbuf) end
    end
    ezio_buffer_destroy(sendbuf)
end

function handle_message(conn, buf, len)
    local pt = buf:ReadAsInt()
    cxlog_info('handle_message ' .. pt)
    if pt == PROTO_JSON then
        local type = buf:ReadAsInt()
        local jslen = buf:ReadAsInt()
        local js = buf:ReadAsString(jslen)
        cxlog_info(jslen .. "  js " .. js)
        if type == PT_TYPE_ARKIT_IPHONE_REG then
            on_iphone_reg(js, conn)
        elseif type == PT_TYPE_ARKIT_U3D_REG then
            on_u3d_reg(js, conn)
        end
    elseif pt == PROTO_BYTES then
        local type = buf:ReadAsInt()
        if type == PT_TYPE_ARKIT_IPHONE_FACE_FRAME then
            on_face_frame(buf, conn, len - 8)
        end
    elseif pt == PROTO_HEART_BEAT then
        local heartSendTime = buf:ReadAsInt64()
        cxlog_info('heartbeat ' .. heartSendTime)
        local sendbuf = ezio_buffer_create()
        sendbuf:WriteInt(PROTO_HEART_BEAT)
        local sz = sendbuf:readable_size()
        sendbuf:PrependInt(sz)
        conn:Send(sendbuf)
        ezio_buffer_destroy(sendbuf)
    end
end

--[[发送
每10帧更新一次，把读到的最新帧发送给客户端，客户端收到后按帧时间回放
]]
function send_frame_update()
    if send_to_u3d then
        local sent_frames = {}
        for p_id, p_info in pairs(phone_infos) do
            for pi, frame in ipairs(p_info.frames) do
                local finfo = {}
                finfo.id = p_id
                finfo.frame = frame
                table.insert(sent_frames, finfo)
            end
        end
        ---清空收到的frames
        for p_id, p_info in pairs(phone_infos) do p_info.frames = {} end

        local buf = ezio_buffer_create()
        buf:WriteInt(PROTO_BYTES)
        buf:WriteInt(PT_TYPE_ARKIT_U3D_FACE_FRAME)
        buf:WriteInt(#sent_frames)
        for i, finfo in ipairs(sent_frames) do
            buf:WriteInt(finfo.id)
            buf:WriteInt64(finfo.frame.tm)
            for bi, val in ipairs(finfo.frame.bs) do
                buf:WriteFloat(val)
            end
        end

        local sz = buf:readable_size()
        buf:PrependInt(sz)
        for u_id, u_info in pairs(u3d_infos) do
            local conn = u_info.conn
            if conn and conn:connected() then conn:Send(buf) end
        end
        ezio_buffer_destroy(buf)
        send_to_u3d = false

        cxlog_info('send_frame_update ')
    end
end

do
    at_exit_manager_init()
    io_service_context_init()

    local dbg_port = command_arg_opt_int('dbg_port', 9403)
    luadbg_listen(dbg_port)

    event_loop = ez_event_loop_create()
    cx_server = ez_tcp_server_create(event_loop, 2701, 'ARKit')

    shared_netq = net_thread_queue_create()

    cx_server:set_on_connection(function(conn)
        cxlog_info('conn-connected : ', conn:connected(), conn:tohostport())
        if conn:connected() then
            conn:SetTCPNoDelay(true)
            table.insert(connections, conn)
        else
            for i, connection in ipairs(connections) do
                if connection == conn then
                    for u_id, u_info in pairs(u3d_infos) do
                        if u_info.conn == connection then
                            u_info.conn = nil
                            u3d_infos[u_id] = nil
                            break
                        end
                    end

                    for p_id, p_info in pairs(phone_infos) do
                        if p_info.conn == connection then
                            p_info.conn = nil
                            phone_infos[p_id] = nil
                            send_phone_info_to_u3d()
                            break
                        end
                    end
                    table.remove(connections, i)
                    break
                end
            end
        end
    end)

    cx_server:set_on_message(function(conn, buf, ts)
        while buf:readable_size() >= CX_MSG_HEADER_LEN do
            local len = buf:PeekAsInt()
            -- cxlog_info('len : ', len)
            if buf:readable_size() >= len + CX_MSG_HEADER_LEN then
                buf:Consume(CX_MSG_HEADER_LEN)
                pcall(handle_message, conn, buf, len)
            else
                break
            end
        end
    end)

    cx_server:Start()
    event_loop:Run()
end

