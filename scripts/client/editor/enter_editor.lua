local AccountSB = imgui.CreateStrbuf('oceancx',256)
local PasswordSB = imgui.CreateStrbuf('123456',256)

local IPSB = imgui.CreateStrbuf('127.0.0.1',256)
local PortSB = imgui.CreateStrbuf('45000',256)

local PosX = imgui.CreateStrbuf('200',128)
local PosY = imgui.CreateStrbuf('2790',128)
local show_demo = false

function on_enter_editor_update()    
    imgui.Begin('Splash')
    local res 
    ret, show_demo = imgui.Checkbox('Demo', show_demo)
    if show_demo then
        imgui.ShowDemoWindow(show_demo)
    end
    imgui.Text('IP  :')
    imgui.SameLine()
    imgui.InputText('##IP', IPSB)

    imgui.Text('Port  :')
    imgui.SameLine()
    imgui.InputText('##Port', PortSB)

    if imgui.Button('连接服务器') then
        local ip = IPSB:str()
        local port = math.tointeger(PortSB:str())
        net_manager_deinit()
        net_manager_init(ip, port)
    end

    imgui.Text("Account   :");
	imgui.SameLine();
    imgui.InputText("##account", AccountSB);

    imgui.Text("Password   :");
	imgui.SameLine();
    imgui.InputText("##password", PasswordSB);
        
    imgui.Text("Pos :");
    imgui.SameLine()
    imgui.PushItemWidth(80)
    imgui.InputText("##posX", PosX)
    imgui.SameLine()
    imgui.InputText("##posY", PosY)
    imgui.PopItemWidth()
    
    if imgui.Button("Sigin Up") then 
        local msg = {}
        msg.account = AccountSB:str()
        msg.password = PasswordSB:str()
        net_send_message(PTO_C2C_SIGNUP, cjson.encode(msg))
    end
    imgui.SameLine()
    if imgui.Button("Login") then
        local msg = {}
        msg.account = AccountSB:str()
        msg.password = PasswordSB:str()
        msg.name = 'oceancx'
        msg.scene_id = 1135
        msg.role_id = 1
        msg.weapon_id = 40
        msg.x = 200
        msg.y = 2790
        net_send_message(PTO_C2C_LOGIN, cjson.encode(msg))
    end
    imgui.SameLine()
    
    if imgui.Button("刷新角色数据库") then
        local player = actor_manager_fetch_local_player()
        local msg = {}
        msg.pid = player:GetID()
        net_send_message(PTO_C2C_SAVE_PLAYER_DATABASE, cjson.encode(msg))
    end
    

    if imgui.Button('重连服务器') then
        net_manager_reconnect()
    end
    if imgui.Button("EnterBattle") then
        scene_manager_switch_scene_by_name('BattleScene')
    end
    
    if imgui.Button('Reload') then 
        -- script_system_dofile('scene/test_net_scene.lua')
        script_system_dofile('utils.lua')
        script_system_dofile('scene/test_net_scene.lua')
        
        debug.sethook()
        script_system_dofile('debugger.lua')
    end

    if imgui.Button('printEnv') then
        utils_dump_table(_ENV)
    end
    imgui.End()
end


