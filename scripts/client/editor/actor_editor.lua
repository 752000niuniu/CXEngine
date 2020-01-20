local AccountSB = imgui.CreateStrbuf('oceancx',256)
local PasswordSB = imgui.CreateStrbuf('123456',256)
local SourceSB = imgui.CreateStrbuf('print("hello")',2560)
local PlayerNameSB = imgui.CreateStrbuf('Ocean藏心',256)

local IPSB = imgui.CreateStrbuf('127.0.0.1',256)
local PortSB = imgui.CreateStrbuf('45000',256)

local PosX = imgui.CreateStrbuf('200',128)
local PosY = imgui.CreateStrbuf('2790',128)
local show_demo = false


local LoginDebugButtons = {
    {
        name  = 'Sign Up',
        on_click = function()
            local msg = {}
            msg.account = AccountSB:str()
            msg.password = PasswordSB:str()
            net_send_message(PTO_C2C_SIGNUP, cjson.encode(msg))
        end
    },
    {
        name  = 'Sign In',
        on_click = function()
            local msg = {}
            msg.account = AccountSB:str()
            msg.password = PasswordSB:str()
            net_send_message(PTO_C2C_LOGIN, cjson.encode(msg))
        end
    },
}

local LocalPlayerDebugButtons = {
    {
        name = 'EnterBattleServer',
        on_click = function()
            local player = actor_manager_fetch_local_player()
            if not player then return end
            
            
            local msg = { atks = {player:GetID()}} 
            net_send_message(PTO_C2S_COMBAT_START, cjson.encode(msg) )
            -- combat_system_start_battle({player},enemys)
        end
    },
    {
        name = 'Say',
        on_click = function()
            local player = actor_manager_fetch_local_player()
            if not player then return end
            player:Say('what the fuck')
        end
    },
    {
        name = 'SetName',
        on_click = function()
            local player = actor_manager_fetch_local_player()
            player:SetProperty(PROP_NAME,PlayerNameSB:str())
        end
    },
    {
        name = 'SetScene',
        on_click = function()
            local player = actor_manager_fetch_local_player()
            player:SetProperty(PROP_SCENE_ID,scene_manager_get_current_scene_id())
        end
    },
    {
        name = 'BoundingBox',
        on_click = function()
            local player = actor_manager_fetch_local_player()
            local show = player:GetProperty(PROP_SHOW_BOUNDINGBOX) 
            player:SetProperty(PROP_SHOW_BOUNDINGBOX , not show)
        end
    },
    {
        name = 'AvatarInfo',
        on_click = function()
            local player = actor_manager_fetch_local_player()
            local show = player:GetProperty(PROP_SHOW_AVATAR_INFO) 
            player:SetProperty(PROP_SHOW_AVATAR_INFO , not show)
        end
    },
    {
        name = 'NormalAttack',
        on_click  = function()
            local player = actor_manager_fetch_local_player()
            local players = actor_manager_fetch_all_actors()
            player:GetTarget():SetProperty(PROP_ASM_BEHIT_ANIM, res_encode(ADDONWDF,0x1D3FF13C ))
            player:PlayAttack()

        end
    },
    {
        name = 'CritAttack',
        on_click  = function()
            local player = actor_manager_fetch_local_player()
            local players = actor_manager_fetch_all_actors()
            player:GetTarget():SetProperty(PROP_ASM_BEHIT_ANIM, res_encode(ADDONWDF, 0xECD0E003))
            player:SetProperty(PROP_ASM_BUFF_ANIM, 0)
            player:PlayAttack()
        end
    },
    {
        name = '横扫千军',
        on_click  = function()
            local player = actor_manager_fetch_local_player()
            local players = actor_manager_fetch_all_actors()
            player:GetTarget():SetProperty(PROP_ASM_BEHIT_ANIM, res_encode(MAGICWDF, 0xACA4A54A))            
            player:SetProperty(PROP_ASM_ATK_COMBO, 3)
            player:SetProperty(PROP_ASM_BUFF_ANIM, res_encode(WADDONWDF,0xD9463A0C))
            player:PlayAttack()
        end
    },
    {
        name = '催眠符',
        on_click  = function()
            local player = actor_manager_fetch_local_player()
            player:SetProperty(PROP_ASM_PLAY_BEHIT, false)
            player:SetProperty(PROP_CAST_ID, res_encode(MAGICWDF, 0x9EC0624E))
            player:SetProperty(PROP_ASM_BUFF_ANIM, res_encode(WADDONWDF,0xCA8FDEAD))
            player:PlayCast()
        end
    },
    {
        name = '善恶有报',
        on_click  = function()
            local player = actor_manager_fetch_local_player()
            player:GetTarget():SetProperty(PROP_ASM_BEHIT_ANIM, res_encode(ADDONWDF, 0x8D8A818D))
            player:SetProperty(PROP_ASM_BUFF_ANIM, 0)
            player:PlayAttack()
        end
    },
    {
        name = '升级',
        on_click  = function()
            local player = actor_manager_fetch_local_player()
            local anim = animation_create(ADDONWDF,0x9B3AF4E5) 
            anim:SetLoop(-1)
            player:AddFrontAnim(anim)
        end
    }
}


     
local cur_add_plan = 1
local add_plan_map ={
    [1] = "1,0,3,1,0", -- 3力1体1耐
    [2] = "0,4,0,1,0", -- 4魔1耐
    [3] = "2,0,0,2,1", -- 2体2耐1敏
    [4] = "1,0,0,1,3", -- 3敏1体1耐
    -- [5] = "0,0,5,0,0", -- 5力
    [5] = "1,0,4,0,0", -- 4力1体
}

function on_actor_editor_update()    
    local res 
    imgui.Begin('Actor编辑器')
    ret, show_demo = imgui.Checkbox('Demo', show_demo)
    if show_demo then
        imgui.ShowDemoWindow(show_demo)
    end
    imgui.SameLine()
   

    imgui.Text("Pos :");
    imgui.SameLine()
    imgui.PushItemWidth(80)
    imgui.InputText("##posX", PosX)
    imgui.SameLine()
    imgui.InputText("##posY", PosY)
    imgui.PopItemWidth()

    imgui.InputText("玩家名字", PlayerNameSB);
    if imgui.CollapsingHeader('Login') then 
        
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

        imgui.Text('Server:')
        imgui.InputTextMultiline('##source', SourceSB, 400, 200,ImGuiInputTextFlags_AllowTabInput)
        if imgui.Button('服务端执行') then
            local code = SourceSB:str()
            net_manager_player_dostring(code)
        end

        imgui_std_horizontal_button_layout(LoginDebugButtons,function(t,k) 
            local nk,v = next(t,k)
            return nk,v, nk and t[nk].name
        end,function(k,v)
            v.on_click()
        end)
    end

    if imgui.CollapsingHeader('Players') then
        local players = actor_manager_fetch_all_actors()
        imgui.HorizontalLayout(players,next,function(k,v) 
            if imgui.Button(v:GetProperty(PROP_AVATAR_ID)..'##'..v:GetID()) then
                actor_manager_set_local_player(v:GetID())
            end
        end)
        if imgui.Button('Clear') then
            actor_manager_clear_all()
        end
    end  
    
    if imgui.CollapsingHeader('Targets') then
        local players = actor_manager_fetch_all_actors()
        imgui.HorizontalLayout(players,next,function(k,v) 
            if imgui.Button(v:GetProperty(PROP_AVATAR_ID)..'##Targets'..v:GetID()) then
                local player = actor_manager_fetch_local_player()
                player:SetTarget(v)
            end
        end)
    end  

    if imgui.CollapsingHeader('LocalPlayer') then
        imgui_std_horizontal_button_layout(LocalPlayerDebugButtons,function(t,k) 
            local nk,v = next(t,k)
            return nk,v, nk and t[nk].name
        end,function(k,v)
            v.on_click()
        end)
    end

    

    imgui.End()
end


