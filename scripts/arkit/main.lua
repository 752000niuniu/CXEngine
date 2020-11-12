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

local phone_infos = {}
function on_iphone_reg(js, conn)
    local info = cjson.decode(js)
    local phone_id = utils_next_uid('phone_id')
    info.id = phone_id
    info.conn = conn
    if phone_infos[info.id] == nil then phone_infos[info.id] = {} end
    phone_infos[info.id].frames = {}
    phone_infos[info.id] = info

    local msg = {}
    msg.id = phone_id
    net_send_jsonobj(conn, PT_TYPE_ARKIT_IPHONE_REG_OK, msg)
end

local u3d_infos = {}
function on_u3d_reg(js, conn)
    local info = cjson.decode(js)
    local u3d_id = utils_next_uid('u3d_id')
    info.id = u3d_id
    info.conn = conn
    u3d_infos[info.id] = info

    local msg = {}
    msg.id = u3d_id
    net_send_jsonobj(conn, PT_TYPE_ARKIT_U3D_REG_OK, msg)
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
]] --
 
function on_face_frame(buf, conn)
    local id = buf:ReadAsInt() 
    local tm = buf:ReadAsInt64() 
    local frame = {}
    frame.tm = tm
    frame.bs = {}

    for i = 1, 52 do
        local bsloc = buf:ReadAsInt()
        local val = buf:ReadAsFloat()
        frame.bs[bsloc] = val
    end
    
    if phone_infos[id].frames == nil then phone_infos[id].frames = {} end
    table.insert(phone_infos[id].frames, frame)
    send_to_u3d = true
end

function handle_message(conn, buf, len)
    local pt = buf:ReadAsInt() 
    local type = buf:ReadAsInt()
    if pt == PROTO_JSON then
        local jslen = buf:ReadAsInt()
        local js = buf:ReadAsString(jslen)
        cxlog_info(jslen .. "  js " .. js)
        if type == PT_TYPE_ARKIT_IPHONE_REG then
            on_iphone_reg(js, conn)
        elseif type == PT_TYPE_ARKIT_U3D_REG then
            on_u3d_reg(js, conn)
        end
    elseif pt == PROTO_BYTES then
        if type == PT_TYPE_ARKIT_IPHONE_FACE_FRAME then
            on_face_frame(buf ,conn) 
        end
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
        for p_id, p_info in pairs(phone_infos) do p_info.frames = {} end

        local buf = ezio_buffer_create()
        buf:WriteInt(PROTO_BYTES)
        buf:WriteInt(PT_TYPE_ARKIT_U3D_FACE_FRAME)
        buf:WriteInt(#sent_frames)
        for i, finfo in ipairs(sent_frames) do
            buf:WriteInt(finfo.id)
            buf:WriteInt64(finfo.frame.tm)
            for bi = 1, 52 do 
                buf:WriteFloat(finfo.frame.bs[bi-1]) 
                -- cxlog_info(BSEnums[bi] .. ' '  ..finfo.frame.bs[bi-1])
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
    end
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
        if conn:connected() then
            conn:SetTCPNoDelay(true)
            table.insert(connections, conn)
        else
            for i, connection in ipairs(connections) do
                if connection == conn then
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
                pcall(handle_message,conn, buf, len)
            else
                break
            end
        end
    end)

    cx_server:Start()
    -- event_loop:RunTaskEvery(ui_update, 30)

    event_loop:RunTaskEvery(send_frame_update, 16)

    -- iw_init(800, 600)
    -- iw_set_font(vfs_get_workdir() .. '/res/font/simsun.ttc')

    event_loop:Run()
    -- iw_deinit()
end

