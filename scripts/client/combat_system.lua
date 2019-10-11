local BATTLE_DEFAULT = 0
local BATTLE_PREPARE = 1
local BATTLE_START = 2
local BATTLE_TURN_STAND_BY = 3
local BTTALE_TURN_EXECUTE = 4
local BTTALE_TURN_NEXT = 5
local BATTLE_END = 6

local COMMAND_STATE_PREPARE = 1
local COMMAND_STATE_PALY = 2
local COMMAND_STATE_STOP = 3

local COMMAND_TYPE_ATTACK = 1
local COMMAND_TYPE_CAST = 2

local CurrentTurn = 1
local Commands = {}
local BattleState = BATTLE_DEFAULT
local tAttackers = {}
local tDefenders = {}

local CommandMT = {}
function CommandMT:new(o)
    o = o or {}
    self.__index = self 
    setmetatable(o, self)
    return o
end

function CommandMT:Init(actor)
    self.state = COMMAND_STATE_PREPARE
    self.type =  COMMAND_TYPE_ATTACK
    self.master = actor
end

function CommandMT:GetState()
    return self.state
end

function CommandMT:GetMaster()
    return self.master
end

function CommandMT:GetType()
    return self.type
end

function CommandMT:StartCast()
    local actor = self.master
    actor:GetTarget():SetProperty(PROP_ASM_BEHIT_ANIM, res_encode(ADDONWDF,0x1D3FF13C ))
    actor:PlayAttack()
end

function CommandMT:Update()
    if self.state == COMMAND_STATE_PREPARE then
        self:StartCast()
        self.state = COMMAND_STATE_PALY
    elseif self.state == COMMAND_STATE_PALY then
    
    elseif self.state == COMMAND_STATE_STOP then
        
    end    
end

function CommandMT:IsFinished()
    return self.state == COMMAND_STATE_STOP
end

local CastCommandMT = CommandMT:new()
function CastCommandMT:Init(actor)
    self.state = COMMAND_STATE_PREPARE
    self.type =  COMMAND_TYPE_CAST
    self.master = actor
end

function CastCommandMT:StartCast()
    local actor = self.master
    actor:PlayCast()
end

-- local AttackCommandMT = CommandMT:new()

BattleBG = BattleBG or nil
combat_self_pos = combat_self_pos or {}
combat_enemy_pos = combat_enemy_pos or {}
function calc_combat_self_pos(ratio_x, ratio_y)
	return {
		{ x = 415.0 * ratio_x, y = 275.0 * ratio_y },
		{ x = 355.0 * ratio_x, y = 305.0 * ratio_y },
		{ x = 475.0 * ratio_x, y = 245.0 * ratio_y },
		{ x = 295.0 * ratio_x, y = 335.0 * ratio_y },
		{ x = 535.0 * ratio_x, y = 215.0 * ratio_y },
		{ x = 465.0 * ratio_x, y = 315.0 * ratio_y },
		{ x = 405.0 * ratio_x, y = 345.0 * ratio_y },
		{ x = 525.0 * ratio_x, y = 285.0 * ratio_y },
		{ x = 345.0 * ratio_x, y = 375.0 * ratio_y },
		{ x = 585.0 * ratio_x, y = 255.0 * ratio_y },
	}
end

function calc_combat_enemy_pos(ratio_x, ratio_y)
	return {
		{ x = 175.0 * ratio_x, y = 170.0 * ratio_y },   
		{ x = 115.0 * ratio_x, y = 200.0 * ratio_y },   
		{ x = 235.0 * ratio_x, y = 140.0 * ratio_y },   
		{ x = 55.0  * ratio_x, y = 230.0 * ratio_y },  
		{ x = 295.0 * ratio_x, y = 110.0 * ratio_y },   
		{ x = 220.0 * ratio_x, y = 210.0 * ratio_y },   
		{ x = 160.0 * ratio_x, y = 240.0 * ratio_y },   
		{ x = 280.0 * ratio_x, y = 180.0 * ratio_y },   
		{ x = 100.0 * ratio_x, y = 270.0 * ratio_y },   
		{ x = 340.0 * ratio_x, y = 150.0 * ratio_y },   
	}
end

function check_turn_ready()
    local ready = true
    for i,actor in ipairs(tDefenders) do
        if not actor:GetProperty(PROP_TURN_READY) then
            ready = false
        end
    end

    for i,actor in ipairs(tAttackers) do
        if not actor:GetProperty(PROP_TURN_READY) then
            ready = false
        end
    end
    return ready
end

function combat_system_init()
    BattleBG = animation_create(ADDONWDF, 0xE3B87E0F)
    local ratio_x = game_get_width()/ 640 
	local ratio_y = game_get_height()/ 480
    combat_self_pos =  calc_combat_self_pos(ratio_x, ratio_y)
	combat_enemy_pos =  calc_combat_enemy_pos(ratio_x, ratio_y)
end

function combat_system_on_start()
    ui_renderer_clear()
    local init_actor = function(actor, pos, dir)
        actor:SetProperty(PROP_IS_COMBAT,true)
        actor:SetProperty(PROP_TURN_READY,false)
        actor:SetProperty(PROP_COMBAT_POS,pos.x,pos.y)
        actor:MoveTo(pos.x,pos.y)
        actor:SetActionID(ACTION_BATIDLE)
        actor:SetDir(dir)
    end

    for i,actor in ipairs(tDefenders) do
        local pos =  combat_enemy_pos[i]
        init_actor(actor, pos, DIR_SE)
    end

    for i,actor in ipairs(tAttackers) do
        local pos =  combat_self_pos[i]
        init_actor(actor, pos, DIR_NW)
    end
end

function combat_system_on_end()
    for i,actor in ipairs(tDefenders) do
        actor:SetProperty(PROP_ASM_BUFF_ANIM, 0)
        actor:ClearBuffAnim()
        actor:SetActionID(ACTION_IDLE)
        actor:SetProperty(PROP_IS_COMBAT,false)
        actor:SetProperty(PROP_CAN_MOVE,true)
    end

    for i,actor in ipairs(tAttackers) do
        actor:SetProperty(PROP_ASM_BUFF_ANIM, 0)
        actor:ClearBuffAnim()
        actor:SetActionID(ACTION_IDLE)
        actor:SetProperty(PROP_IS_COMBAT,false)
        actor:SetProperty(PROP_CAN_MOVE,true)
    end
    animation_manager_clear()
    scene_set_combat(false)
end

function combat_system_update()
    if BattleState == BATTLE_DEFAULT then
        return
    elseif BattleState == BATTLE_START then
        combat_system_on_start()
        BattleState = BATTLE_TURN_STAND_BY
    elseif BattleState == BATTLE_TURN_STAND_BY then
--[[
    每帧update都看player是不是输入了指令
    local player的指令输入来自键盘 或者  鼠标
    而 npc的输入来自随机
    敌对玩家的输入来自网络
]]

        if check_turn_ready() then
            BattleState = BTTALE_TURN_EXECUTE
        end
    elseif BattleState == BTTALE_TURN_EXECUTE then
--[[
    处理每一条战斗指令,
    每条战斗指令,代表某个actor的演出
    处理完战斗指令,所有的actor演出结束
    战斗指令的先后顺序代表actor的先后顺序

    攻击指令
        攻击对象 攻击效果
    施法指令
        
    防御指令
]]
        if #Commands > 0 then
            local cmd = Commands[1]
            cmd:Update()
            if cmd:IsFinished() then
                table.remove(Commands,1)
            end
        else 
            if check_turn_end() then
                BattleState = BTTALE_TURN_END
            else
                BattleState = BTTALE_TURN_NEXT
            end
        end

    elseif BattleState == BTTALE_TURN_NEXT then
        CurrentTurn = CurrentTurn + 1 
        for i,actor in ipairs(tDefenders) do
            actor:SetProperty(PROP_TURN_READY,false)
        end
    
        for i,actor in ipairs(tAttackers) do
            actor:SetProperty(PROP_TURN_READY,false)
        end

        BattleState = BATTLE_TURN_STAND_BY
    elseif BattleState == BTTALE_TURN_END then
        combat_system_on_end()
        BattleState = BATTLE_DEFAULT
        return
    end

	for i,actor in ipairs(tDefenders) do
        actor:Update()
    end

    for i,actor in ipairs(tAttackers) do
        actor:Update()
    end
    animation_manager_update()
end

function combat_system_draw()
    BattleBG:Draw()
    for i,actor in ipairs(tDefenders) do
        actor:Draw()
    end

    for i,actor in ipairs(tAttackers) do
        actor:Draw()
    end
    animation_manager_draw()
end


function combat_system_start_battle(atk_actors, dfd_actors)
    scene_set_combat(true)
    tAttackers = atk_actors
    tDefenders = dfd_actors
    BattleState = BATTLE_START
end

function combat_system_on_acting_end(actor)
    if #Commands > 0 then
       local cmd = Commands[1]
       cmd.state = COMMAND_STATE_STOP
    end
end

function combat_system_imgui_update()
    if BattleState == BATTLE_TURN_STAND_BY then
        if imgui.Button('敌人模拟攻击##player') then
            local player = actor_manager_fetch_local_player()
            local enemy = player:GetTarget()
            enemy:SetTarget(player)

            local cmd = CommandMT:new()
            cmd:Init(enemy)
            table.insert(Commands,cmd)

            enemy:SetProperty(PROP_TURN_READY, true)
        end    

        if imgui.Button('敌人模拟法术攻击##player') then
            local magic_tbl = content_system_get_table('magic')
            local keys = utils_fetch_sort_keys(magic_tbl)
            local cast_id = magic_tbl[keys[math.random(1,#keys)]].resid

            local player = actor_manager_fetch_local_player()
            local enemy = player:GetTarget()
            enemy:SetTarget(player)
            
            enemy:SetProperty(PROP_ASM_PLAY_BEHIT, true)
            enemy:SetProperty(PROP_CAST_ID, res_encode(MAGICWDF, cast_id))
            enemy:SetProperty(PROP_ASM_BUFF_ANIM, 0)

            local cmd = CastCommandMT:new()
            cmd:Init(enemy)
            table.insert(Commands,cmd)

            enemy:SetProperty(PROP_TURN_READY, true)
        end

        if imgui.Button('攻击##player') then
            local player = actor_manager_fetch_local_player()
            
            local cmd = CommandMT:new()
            cmd:Init(player)
            table.insert(Commands,cmd)

            player:SetProperty(PROP_TURN_READY, true)
        end  
        
        if imgui.Button('法术攻击##player') then
            local magic_tbl = content_system_get_table('magic')
            local keys = utils_fetch_sort_keys(magic_tbl)
            local cast_id = magic_tbl[keys[math.random(1,#keys)]].resid

            local player = actor_manager_fetch_local_player()
            
            player:SetProperty(PROP_ASM_PLAY_BEHIT , true)
            player:SetProperty(PROP_CAST_ID, res_encode(MAGICWDF, cast_id))
            player:SetProperty(PROP_ASM_BUFF_ANIM,0)

            local cmd = CastCommandMT:new()
            cmd:Init(player)
            table.insert(Commands,cmd)

            player:SetProperty(PROP_TURN_READY, true)
        end  
        
    end
	imgui.Begin('Menu',menu_show)


    if imgui.Button('法术##player') then

    end

    if imgui.Button('特技##player') then

    end

    if imgui.Button('道具##player') then

    end

    if imgui.Button('防御##player') then

    end

    if imgui.Button('召唤##player') then

    end

    if imgui.Button('召还##player') then

    end

    if imgui.Button('捕捉##player') then

    end

    if imgui.Button('逃跑##player') then
        combat_system_on_end()
        scene_set_combat(false)
    end
    imgui.End()

    imgui.Begin('BBMenu',bb_menu_show)
    if imgui.Button('法术##bb') then

    end

    if imgui.Button('道具##bb') then

    end

    if imgui.Button('防御##bb') then

    end

    if imgui.Button('逃跑##bb') then

    end
    imgui.End()

    if  imgui.KeysMod('ALT') and imgui.IsKeyReleased(string.byte('A'))  then
        local player = actor_manager_fetch_local_player()
        player:GetTarget():SetProperty(PROP_ASM_BEHIT_ANIM, res_encode(ADDONWDF,0x1D3FF13C))
        player:PlayAttack()
    end
end

