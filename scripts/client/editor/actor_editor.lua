local AccountSB = imgui.CreateStrbuf('oceancx',256)
local PasswordSB = imgui.CreateStrbuf('123456',256)
local PlayerNameSB = imgui.CreateStrbuf('Ocean藏心',256)

local IPSB = imgui.CreateStrbuf('127.0.0.1',256)
local PortSB = imgui.CreateStrbuf('45000',256)

local PosX = imgui.CreateStrbuf('200',128)
local PosY = imgui.CreateStrbuf('2790',128)
local show_demo = false

local function format_path(path)
    if string.find(path, '@') == 1 then
        path = string.sub(path, 2)
    end
    path = string.gsub(path,'\\','/')
    return string.lower(path)
end

local LoginDebugButtons = {
    {
        name  = 'Sigin Up',
        on_click = function()
            local msg = {}
            msg.account = AccountSB:str()
            msg.password = PasswordSB:str()
            net_send_message(PTO_C2C_SIGNUP, cjson.encode(msg))
        end
    },
    {
        name  = 'Sigin In',
        on_click = function()
            local msg = {}
            msg.account = AccountSB:str()
            msg.password = PasswordSB:str()
            msg.name = 'Ocean藏心'
            msg.scene_id = 1135
            msg.role_id = 1
            msg.weapon_id = 40
            msg.x = 200
            msg.y = 2790
            net_send_message(PTO_C2C_LOGIN, cjson.encode(msg))
        end
    },
    {
        name  = '刷新角色数据库',
        on_click = function()
            local player = actor_manager_fetch_local_player()
            local msg = {}
            msg.pid = player:GetID()
            net_send_message(PTO_C2C_SAVE_PLAYER_DATABASE, cjson.encode(msg))
        end
    },
    {
        name  = '重连服务器',
        on_click = function()
            net_manager_reconnect()
        end
    },
    {
        name  = '监听调试器',
        on_click = function()
            luadbg_stop()
            luadbg_listen(9527) 
        end
    },
    {
        name  = '启动DebugAdapter',
        on_click = function()
            local path = vfs_makepath('internals/luadebugger/vscode/Debug/vsdbgadapter.exe')
            local cwd = vfs_makepath('internals/luadebugger/vscode/')
            cwd = format_path(cwd) 
            local cmd = string.format('start %s --cwd=%s',path,cwd )
            os.execute(cmd)
        end
    },
    {
        name  = '启动服务器',
        on_click = function()
            local path = vfs_makepath('bin/Debug/SimpleServer.exe')
            local cwd = vfs_makepath('') 
            cwd = format_path(cwd) 
            local cmd = string.format('start %s --cwd=%s',path,cwd )
            os.execute(cmd)
        end
    },
    {
        name  = '安装vscode插件',
        on_click = function()
            local dir = vfs_makepath('')
            local cmd = string.format('sh  %sinternals/luadebugger/install_extension.sh %s',dir,dir)
            print(cmd)
            os.execute(cmd)
        end
    },
    {
        name  = '重新生成ActorProp',
        on_click = function()
            script_system_dofile('generator/actor_template.lua')
        end
    }
}

local LocalPlayerDebugButtons = {
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
            local players = actor_manager_fetch_all_players()
            player:GetTarget():SetProperty(PROP_ASM_BEHIT_ANIM, res_encode(ADDONWDF,0x1D3FF13C ))
            player:PlayAttack()

        end
    },
    {
        name = 'CritAttack',
        on_click  = function()
            local player = actor_manager_fetch_local_player()
            local players = actor_manager_fetch_all_players()
            player:GetTarget():SetProperty(PROP_ASM_BEHIT_ANIM, res_encode(ADDONWDF, 0xECD0E003))
            player:SetProperty(PROP_ASM_BUFF_ANIM, 0)
            player:PlayAttack()
        end
    },
    {
        name = '横扫千军',
        on_click  = function()
            local player = actor_manager_fetch_local_player()
            local players = actor_manager_fetch_all_players()
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
    if imgui.Button('Reload') then
        actor_manager_clear_all()
        scene_manager_reload()
        script_system_dofile('../share/utils.lua')
        script_system_dofile('editor/imgui_editor.lua')
        script_system_dofile('ui_renderer.lua')
        script_system_dofile('combat_system.lua')
        script_system_dofile('input_manager.lua')
        collectgarbage()
    end

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

        imgui_std_horizontal_button_layout(LoginDebugButtons,function(t,k) 
            local nk,v = next(t,k)
            return nk,v, nk and t[nk].name
        end,function(k,v)
            v.on_click()
        end)
    end

    if imgui.CollapsingHeader('Players') then
        local players = actor_manager_fetch_all_players()
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
        local players = actor_manager_fetch_all_players()
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

    if imgui.CollapsingHeader('ActorInfo') then
        local actor = actor_manager_fetch_local_player()
        if actor then
            local actor_type = actor:GetProperty(PROP_ACTOR_TYPE) 
            for i = 1, 5 do 
                local selected = imgui.RadioButton(i..'##rbplan',cur_add_plan == i) 
                if i ~= 5 then
                    imgui.SameLine()
                end
                if selected then
                    cur_add_plan = i
                    actor:SetProperty(PROP_ADD_PROP_PLAN, add_plan_map[cur_add_plan])
                    cxlog_info('cur_add_plan',cur_add_plan,add_plan_map[cur_add_plan])
                end
            end

            imgui.Text('等级 '..actor:GetProperty(PROP_LV))
            imgui.SameLine()
            if imgui.Button('+5##lv') then
                local lv = actor:GetProperty(PROP_LV) + 5
                actor:SetProperty(PROP_LV, lv)
                if actor==ACTOR_TYPE_PLAYER then
                    actor:UpdatePropPtsByPlan()
                end
            end
            imgui.SameLine()
            if imgui.Button('-5##lv') then
                local lv = actor:GetProperty(PROP_LV) - 5
                actor:SetProperty(PROP_LV, lv)
                if actor==ACTOR_TYPE_PLAYER then
                    actor:UpdatePropPtsByPlan()
                end
            end

            if imgui.Button('固定计算') then
                if actor_type==ACTOR_TYPE_SUMMON then
                    actor:SetProperty(PROP_SUMMON_ATK_QUAL ,1400)
                    actor:SetProperty(PROP_SUMMON_DEF_QUAL ,1400)
                    actor:SetProperty(PROP_SUMMON_HEALTH_QUAL ,4500)
                    actor:SetProperty(PROP_SUMMON_MAGIC_QUAL , 2500)
                    actor:SetProperty(PROP_SUMMON_SPEED_QUAL ,1200)
                    actor:SetProperty(PROP_SUMMON_DODGE_QUAL , 1200)
                    actor:SetProperty(PROP_SUMMON_GROW_COEF , 1.25)

                    actor:SetProperty(PROP_BASE_HEALTH, 417 )
                    actor:SetProperty(PROP_BASE_MAGIC,680 )
                    actor:SetProperty(PROP_BASE_FORCE, 171)
                    actor:SetProperty(PROP_BASE_STAMINA , 171 )
                    actor:SetProperty(PROP_BASE_AGILITY, 171)
                    actor:SetProperty(PROP_LV, 151)
                else
                    actor:SetProperty(PROP_BASE_HEALTH, 333 )
                    actor:SetProperty(PROP_BASE_MAGIC,157 )
                    actor:SetProperty(PROP_BASE_FORCE, 689)
                    actor:SetProperty(PROP_BASE_STAMINA , 215 )
                    actor:SetProperty(PROP_BASE_AGILITY, 157)
                    actor:SetProperty(PROP_LV, 145)
                end
            end

            imgui.Text('MaxHP ' .. actor:GetMaxHP())
            imgui.Text('当前HP '..actor:GetProperty(PROP_BASE_HP))

            imgui.Text('MaxMP ' .. actor:GetMaxMP())
            imgui.Text('当前MP '..actor:GetProperty(PROP_BASE_MP))

            imgui.Text('当前SP '..actor:GetProperty(PROP_BASE_SP))
            
            imgui.Text('体质 '..actor:GetProperty(PROP_BASE_HEALTH))
            imgui.Text('魔力 '..actor:GetProperty(PROP_BASE_MAGIC))
            imgui.Text('力量 '..actor:GetProperty(PROP_BASE_FORCE))
            imgui.Text('耐力 '..actor:GetProperty(PROP_BASE_STAMINA))
            imgui.Text('敏捷 '..actor:GetProperty(PROP_BASE_AGILITY))

            
            imgui.Text('命中 '..actor:CalcTargetHit())
            imgui.Text('攻击 '..actor:CalcAttack())
            imgui.Text('防御 '..actor:CalcDefend())
            imgui.Text('灵力 '..actor:CalcSpiritual())
            imgui.Text('速度 '..actor:CalcSpeed())
            imgui.Text('躲闪 '..actor:CalcDodge())
        end
    end

    imgui.End()
end


