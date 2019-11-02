dofile(vfs_get_luapath('../combat/skill.lua') )
dofile(vfs_get_luapath('../combat/buffer.lua') )

PERFRAME_TIME = 0.016*2.5  
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

local TEAM_TYPE_ATTACKER = 1
local TEAM_TYPE_DEFENDER = 2

local ACTOR_CLICK_MODE_ATTACK = 1
local ACTOR_CLICK_MODE_SPELL = 2
local ACTOR_CLICK_MODE  = ACTOR_CLICK_MODE_ATTACK

local CurrentTurn = 1
local Commands = {}
local BattleState = BATTLE_DEFAULT

local skill_template_table  = {}
local buffer_template_table = {}

local BufferMT = {}
function BufferMT:new(o)
    o = o or {
    }
    self.__index = self 
    setmetatable(o, self)
    return o
end


local TeamMT = {}
function TeamMT:new(o)
    o = o or {
        members={}
    }
    self.__index = self 
    setmetatable(o, self)
    return o
end

function TeamMT:GetID()
    return self.id
end

function TeamMT:SetType(type)
    self.type = type
end

function TeamMT:GetType()
    return self.type 
end

function TeamMT:AddMember(actor)
    table.insert(self.members,actor:GetID())
end

function TeamMT:RemoveMember(actor)
    for i,id in ipairs(self.members) do
        if id == actor:GetID() then
            table.remove(self.members,i)
            return 
        end
    end
end

function TeamMT:GetGroupKillTargets(count)
    local targets = {}
    for i=1,count do
        table.insert(targets, self.members[i])
    end
    return targets
end

function TeamMT:IsReady()
    local ready = true
    for i,id in ipairs(self.members) do
        local actor = actor_manager_fetch_player_by_id(id)
        if not actor:GetProperty(PROP_TURN_READY) then
            ready = false
            break
        end
    end
    return ready
end

function TeamMT:SetReady()
    for i,id in ipairs(self.members) do
        local actor = actor_manager_fetch_player_by_id(id)
        actor:SetProperty(PROP_TURN_READY, false)
    end
end


function TeamMT:IsAllDead()
    local all_dead = true
    for i,id in ipairs(self.members) do
        local actor = actor_manager_fetch_player_by_id(id)
        if not actor:IsDead() then
            all_dead = false
            break
        end
    end
    return all_dead
end

function TeamMT:DrawHP()
    for i,id in ipairs(self.members) do
        local actor = actor_manager_fetch_player_by_id(id)
        local x,y,w,h = actor:GetAvatarRect()
        imgui.SetCursorPos(x,y-10)
        local max_hp = actor:GetMaxHP()
        local hp = actor:GetHP()
        imgui.Text(hp..'/'..max_hp)
    end
end

local AttackerTeam 
local DefenderTeam
local ActorBuffers = {}
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
    self.main_target = actor:GetTarget()
    self.targets = { self.main_target }
    self.acting_actors = {}
end


function CommandMT:AddTarget(actor)
    table.insert(self.targets, actor)
end

function CommandMT:SetMainTarget(actor)
    for i, target in ipairs(self.targets) do
        if target == actor then
            self.main_target = actor
            return 
        end
    end
    self:AddTarget(actor)
    self.main_target = actor
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

function CommandMT:AddActing(actor)
    for i,v in ipairs(self.acting_actors) do
        if v:GetID() == actor:GetID() then
            return
        end
    end
    table.insert(self.acting_actors,actor)
end

function CommandMT:RemoveActing(actor)
    for i,v in ipairs(self.acting_actors) do
        if v:GetID() == actor:GetID() then
            table.remove(self.acting_actors, i)
            if #self.acting_actors == 0 then
                self.state = COMMAND_STATE_STOP
            end
            return 
        end
    end
end

function CommandMT:IsActing()
    return #self.acting_actors > 0
end

function CommandMT:StartCast()
    local actor = self.master
    local skill_id = actor:GetProperty(PROP_USING_SKILL)
    local skill = skill_template_table[skill_id]
    on_cast_skill(skill, actor)
end

function CommandMT:Update()
    if self.state == COMMAND_STATE_PREPARE then
        if self.master:IsDead() then
            self.state = COMMAND_STATE_STOP
            return
        end
        self:StartCast()
        self.state = COMMAND_STATE_PALY
    elseif self.state == COMMAND_STATE_PALY then
        
    elseif self.state == COMMAND_STATE_STOP then
        
    end    
end

function CommandMT:IsFinished()
    return self.state == COMMAND_STATE_STOP
end

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
    return AttackerTeam:IsReady() and DefenderTeam:IsReady()
end

function combat_system_init()
    BattleBG = animation_create(ADDONWDF, 0xE3B87E0F)
    local ratio_x = game_get_width()/ 640 
	local ratio_y = game_get_height()/ 480
    combat_self_pos =  calc_combat_self_pos(ratio_x, ratio_y)
    combat_enemy_pos =  calc_combat_enemy_pos(ratio_x, ratio_y)
    skill_template_table = content_system_get_table('skill')
    buffer_template_table = content_system_get_table('buffer')
    init_skills()
    init_buffers()
    -- cxlog_info('combat_system_init',cjson.encode(skill_template_table))
end

function auto_ready_other_actors()
    for i,actor in ipairs(BattleActors) do
        if not actor:IsLocal() then
            -- local player = actor_manager_fetch_local_player()
            -- actor:SetTarget(player)
            -- actor:SetProperty(PROP_USING_SKILL, 1)

            -- local cmd = CommandMT:new()
            -- cmd:Init(actor)
            -- table.insert(Commands,cmd)
            actor:SetProperty(PROP_TURN_READY, true)        
        end
    end
end
    
function combat_system_actor_on_click(actor, button, x, y)
    if BattleState ~= BATTLE_TURN_STAND_BY then return end
    if ACTOR_CLICK_MODE == ACTOR_CLICK_MODE_ATTACK then
        local player = actor_manager_fetch_local_player()
        player:SetTarget(actor)
        player:SetProperty(PROP_USING_SKILL, 1)
    
        local cmd = CommandMT:new()
        cmd:Init(player)
        table.insert(Commands,cmd)
        player:SetProperty(PROP_TURN_READY, true)        
        auto_ready_other_actors()
    elseif ACTOR_CLICK_MODE == ACTOR_CLICK_MODE_SPELL then
        local player = actor_manager_fetch_local_player()
        player:SetTarget(actor)
    
        local cmd = CommandMT:new()
        cmd:Init(player)
        table.insert(Commands,cmd)
        player:SetProperty(PROP_TURN_READY, true)        
        auto_ready_other_actors()
    end

    ACTOR_CLICK_MODE = ACTOR_CLICK_MODE_ATTACK
end


local actor_on_click_cb = {}
function combat_system_on_start()
    ui_renderer_clear()
    local init_actor = function(actor, pos, dir)
        actor:SetProperty(PROP_IS_COMBAT,true)
        actor:SetProperty(PROP_TURN_READY,false)
        actor:SetProperty(PROP_COMBAT_POS,pos.x,pos.y)
        actor:SetDir(dir)
        actor:ClearAction()
        actor:PushAction(ACTION_BATIDLE)

        actor_on_click_cb[actor:GetID()] = actor_reg_event(actor, ACTOR_EV_ON_CLICK, function(actor, button, x, y)
            combat_system_actor_on_click(actor,button,x,y)
        end)
    end

    local player = actor_manager_fetch_local_player()
    local self_team_type = player:GetProperty(PROP_TEAM_TYPE)
    for i,actor in ipairs(BattleActors) do
        if actor:GetProperty(PROP_TEAM_TYPE) == self_team_type then
            local pos = combat_self_pos[i]
            init_actor(actor, pos, DIR_NW)
        else
            local pos =  combat_enemy_pos[i]
            init_actor(actor, pos, DIR_SE)
        end
    end
end

function combat_system_on_end()
    local reset_actor = function(actor)
        actor:SetProperty(PROP_ASM_BUFF_ANIM, 0)
        -- actor:SetActionID(ACTION_IDLE)
        actor:SetProperty(PROP_IS_COMBAT,false)
        actor:SetProperty(PROP_CAN_MOVE,true)
        actor:ClearAction()
        actor:PushAction(ACTION_IDLE)
        local avatar = actor:GetAvatar()
        cxlog_info('combat_system_on_end reset actor',actor:GetProperty(PROP_NAME) , avatar)
        actor_reg_event(actor, ACTOR_EV_ON_CLICK, actor_on_click_cb[actor:GetID()])
    end

    for i,actor in ipairs(BattleActors) do
        reset_actor(actor)
    end
    
    animation_manager_clear()
    scene_set_combat(false)
end


function combat_system_on_turn_end()

end

function check_battle_end()
    return AttackerTeam:IsAllDead() or DefenderTeam:IsAllDead()
end

function combat_system_fetch_current_cmd()
    if #Commands > 0 then
        local cmd = Commands[1]
        return cmd
    end
end

local CurrentCmd = {}
function combat_system_update()
    if BattleState == BATTLE_DEFAULT then
        return
    elseif BattleState == BATTLE_START then
        combat_system_on_start()
        BattleState = BATTLE_TURN_STAND_BY
        cxlog_info('BATTLE_TURN_STAND_BY')
        skill_on_turn(CurrentTurn)
    elseif BattleState == BATTLE_TURN_STAND_BY then
--[[
    每帧update都看player是不是输入了指令
    local player的指令输入来自键盘 或者  鼠标
    而 npc的输入来自随机
    敌对玩家的输入来自网络
]]
        if check_turn_ready() then
            BattleState = BTTALE_TURN_EXECUTE
            cxlog_info('BTTALE_TURN_EXECUTE')
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
            if not cmd.master:IsDead() then
                CurrentCmd = cmd
                cmd:Update()
                if cmd:IsFinished() then
                    table.remove(Commands,1)
                end
            else
                table.remove(Commands,1)
            end
        else 
            if check_battle_end() then
                BattleState = BTTALE_END
                cxlog_info('BTTALE_END')
            else
                BattleState = BTTALE_TURN_NEXT
                cxlog_info('BTTALE_TURN_NEXT')
            end
        end

    elseif BattleState == BTTALE_TURN_NEXT then
        CurrentTurn = CurrentTurn + 1 

        for i,actor in ipairs(BattleActors) do
            actor:SetProperty(PROP_TURN_READY,false)
        end

        skill_on_turn(CurrentTurn)
        
        for i,actor in ipairs(BattleActors) do
            actor:BufferNextTurn(CurrentTurn)
        end


        BattleState = BATTLE_TURN_STAND_BY
        cxlog_info('BATTLE_TURN_STAND_BY')
    elseif BattleState == BTTALE_END then
        combat_system_on_end()
        BattleState = BATTLE_DEFAULT
        cxlog_info('BATTLE_DEFAULT')
        return
    end

    for i,actor in ipairs(BattleActors) do
        actor:Update()
    end
    animation_manager_update()
end

function combat_system_draw()
    BattleBG:Draw()

    local drawActors = {}
    for i,actor in ipairs(BattleActors) do
        table.insert(drawActors,actor)
    end
    table.sort(drawActors,function(a,b) return a:GetY() < b:GetY() end)

    for i,actor in ipairs(drawActors) do
        actor:Draw()
    end

    animation_manager_draw()
end


function combat_system_start_battle(atk_actors, dfd_actors)
    scene_set_combat(true)
    BattleActors = {}

    local team_id = os.time()
    AttackerTeam = TeamMT:new()
    AttackerTeam.id = team_id
    AttackerTeam:SetType(TEAM_TYPE_ATTACKER)
    for i,actor in ipairs(atk_actors) do
        AttackerTeam:AddMember(actor)
        actor:SetProperty(PROP_TEAM_TYPE, TEAM_TYPE_ATTACKER)
        actor:SetProperty(PROP_TEAM_ID, AttackerTeam:GetID())
        table.insert(BattleActors, actor)
    end

    DefenderTeam = TeamMT:new()
    DefenderTeam.id = team_id + 1
    DefenderTeam:SetType(TEAM_TYPE_DEFENDER)
    for i,actor in ipairs(dfd_actors) do
        DefenderTeam:AddMember(actor)
        actor:SetProperty(PROP_TEAM_TYPE, TEAM_TYPE_DEFENDER)
        actor:SetProperty(PROP_TEAM_ID, DefenderTeam:GetID())
        table.insert(BattleActors, actor)
    end
    BattleState = BATTLE_START
end

function actor_get_team(actor)
    if actor:GetProperty(PROP_TEAM_TYPE) == TEAM_TYPE_ATTACKER then
        return AttackerTeam
    elseif actor:GetProperty(PROP_TEAM_TYPE) == TEAM_TYPE_DEFENDER then
        return DefenderTeam
    else
        cxlog_info('no team!!!')
    end
end

function combat_system_remove_from_battle(_actor_)
    local reset_actor = function(actor)
        actor:SetProperty(PROP_ASM_BUFF_ANIM, 0)
        -- actor:SetActionID(ACTION_IDLE)
        actor:SetProperty(PROP_IS_COMBAT,false)
        actor:SetProperty(PROP_CAN_MOVE,true)
        actor:ClearAction()
        actor:PushAction(ACTION_IDLE)
        
        local avatar = actor:GetAvatar()
        cxlog_info('combat_system_remove_from_battle reset actor',actor:GetProperty(PROP_NAME) , avatar)
        actor_reg_event(actor, ACTOR_EV_ON_CLICK, actor_on_click_cb[actor:GetID()])
    end

    for i,actor in ipairs(BattleActors) do
        if actor:GetID() == _actor_:GetID() then
            reset_actor(actor)
            table.remove(BattleActors,i) 
            return
        end
    end
end

function test_local_player_cast(cast_id)
    local player = actor_manager_fetch_local_player()
    player:SetProperty(PROP_USING_SKILL, cast_id)

    local cmd = CommandMT:new()
    cmd:Init(player)

    table.insert(Commands,cmd)

    player:SetProperty(PROP_TURN_READY, true)
    auto_ready_other_actors()
end

function combat_system_imgui_update()
    if BattleState ~= BATTLE_TURN_STAND_BY then return end
	imgui.Begin('Menu',menu_show)

    if imgui.Button('攻击##player') then
        
    end  

    if imgui.Button('法术##player') then
        imgui.OpenPopup('SpellSelector')
        ACTOR_CLICK_MODE = ACTOR_CLICK_MODE_SPELL
        -- test_local_player_cast(51)
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
    if imgui.BeginPopup('SpellSelector') then
        local player = actor_manager_fetch_local_player()
        if imgui.Button('横扫千军') then
            player:SetProperty(PROP_USING_SKILL, 20)
            imgui.CloseCurrentPopup()
        end
        if imgui.Button('催眠符') then
            player:SetProperty(PROP_USING_SKILL, 26)
            imgui.CloseCurrentPopup()
        end
        if imgui.Button('龙腾') then
            player:SetProperty(PROP_USING_SKILL, 38)
            imgui.CloseCurrentPopup()
        end

        imgui.EndPopup('SpellSelector')
    end
    imgui.End()
    -- imgui.Begin('BBMenu',bb_menu_show)
    -- if imgui.Button('法术##bb') then

    -- end

    -- if imgui.Button('道具##bb') then

    -- end

    -- if imgui.Button('防御##bb') then

    -- end

    -- if imgui.Button('逃跑##bb') then

    -- end
    -- imgui.End()

    if  imgui.KeysMod('ALT') and imgui.IsKeyReleased(string.byte('A'))  then
        local player = actor_manager_fetch_local_player()
        player:GetTarget():SetProperty(PROP_ASM_BEHIT_ANIM, res_encode(ADDONWDF,0x1D3FF13C))
        player:PlayAttack()
    end

    DefenderTeam:DrawHP()
    AttackerTeam:DrawHP()
end


function calc_run_to_pos(actor, target)
    local dir = actor:GetDir()
    local attackAvatar = actor:GetAvatar(ACTION_ATTACK)
    attackAvatar:SetDir(dir)
    local targetAvatar = target:GetAvatar(ACTION_BEHIT)
    targetAvatar:SetDir(math_get_reverse_dir(dir))
    if attackAvatar and targetAvatar then
        local actor_avtar_id = actor:GetProperty(PROP_AVATAR_ID)
        local attackKeyframe = action_get_attack_key_frame(actor_avtar_id)
        if attackKeyframe == 0 then attackKeyframe = attackAvatar:GetKeyFrame() end
        local attackFrame = attackKeyframe
        local targetFrame = targetAvatar:GetGroupFrameCount() - 1 
        local x, y = target:GetPos()
        local minus_target_key_y_plus_half_height_minus_attack_half_height = 
            - targetAvatar:GetFrameKeyY(targetFrame) + targetAvatar:GetFrameHeight(targetFrame)*0.5 - attackAvatar:GetFrameHeight(attackFrame)/2
        
        if dir == DIR_NE then
            x = x - attackAvatar:GetFrameWidth(attackFrame) - 5
            y = y + minus_target_key_y_plus_half_height_minus_attack_half_height + 11
        elseif dir == DIR_NW then
            x = x + 5
            y = y + minus_target_key_y_plus_half_height_minus_attack_half_height + 11            
        elseif dir == DIR_SE then
            x = x - attackAvatar:GetFrameWidth(attackFrame) - 5
            y = y + minus_target_key_y_plus_half_height_minus_attack_half_height - 11            
        elseif dir == DIR_SW then
            x = x + 5
            y = y + minus_target_key_y_plus_half_height_minus_attack_half_height - 11            
        end
        
		x = x + attackAvatar:GetFrameKeyX(attackFrame)
		y = y + attackAvatar:GetFrameKeyY(attackFrame)
        local src_x,src_y = actor:GetPos()
        local runto_x = x - src_x
        local runto_y = y - src_y
        return runto_x ,runto_y
    end
end

function on_cast_spell( skill, actor)
    skill_on_start(skill, actor)
    local target = actor:GetTarget()
    local cast_action = actor:GetAvatar(ACTION_CAST)
    cast_action:Reset()
    cast_action:SetLoop(-1)
    cast_action:AddFrameCallback(cast_action:GetGroupFrameCount()/2,function()
        for i,target in ipairs(BattleActors) do
            if target:GetProperty(PROP_TEAM_TYPE) ~= actor:GetProperty(PROP_TEAM_TYPE) then
                local damage = actor:GetSpellDamage(target)
                target:SetProperty(PROP_ASM_DAMAGE,damage) 
                target:ModifyHP(-damage)
                target:SetProperty(PROP_ASM_BEHIT_ANIM, skill.atk_anim)
        
                local behit_action = target:GetAvatar(ACTION_BEHIT)
                behit_action:Reset()
                behit_action:SetLoop(1)
                behit_action:AddFrameCallback(1, function()
                    local avatar = target:GetAvatar()
                    local resid = skill.atk_anim
                    local pack, was = res_decode(resid)
                    local anim = animation_create(pack,was)
                    anim:SetLoop(-1)
                    -- local offy = -avatar:GetFrameKeyY() + avatar:GetFrameHeight() / 2.0
                    -- anim:SetOffsetY(offy)  
                    target:AddFrontAnim(anim)
        
                    local damage = target:GetProperty(PROP_ASM_DAMAGE) 
                    target:ShowBeatNumber(damage)
                    behit_action:Pause(math.floor(anim:GetGroupFrameTime()* 1000))
                end)
        
                behit_action:AddStopCallback(function()
                    skill_on_hit(skill, actor, target)
                    if target:IsDead() then
                        behit_action:Reset()
                        behit_action:Play()
                        behit_action:SetLoop(0)
        
                        local last_x, last_y = target:GetPos()
                        local last_dir = target:GetDir()
                        local dx, dy = 0,0
                        local dir_x ,dir_y = actor:GetAttackVec()
                        local fly_x ,fly_y = 0,0 
                        behit_action:AddUpdateCallback(function()
                            local actor = target
                        
                            local px, py = actor:GetPos()
                            local avatar = actor:GetAvatar()
                            if py - avatar:GetFrameKeyY() <= 0 then
                                dir_y = -dir_y
                            end
        
                            if py - avatar:GetFrameKeyY() + avatar:GetFrameHeight()  >= 600 then
                                dir_y = -dir_y
                            end
                            
                            if avatar:IsFrameUpdate() then
                                px = px + dir_x * 49
                                py = py + dir_y * 49
                                actor:SetProperty(PROP_COMBAT_POS, px,py)
                            end
        
                            if avatar:IsGroupEndUpdate() then
                                local dir = actor:GetDir()
                                dir = math_next_dir4(dir)
                                actor:SetDir(dir)
                            end
                            if px - avatar:GetFrameKeyX() < 0 then
                                behit_action:RemoveUpdateCallback()
                                behit_action:Stop()
                                actor:SetProperty(PROP_COMBAT_POS,last_x,last_y)
                                actor:SetDir(last_dir)
        
                                CurrentCmd:RemoveActing(target)
                                combat_system_remove_from_battle(target)
                            end
        
                            if px - avatar:GetFrameKeyX() + avatar:GetFrameWidth() >= 800 then
                                behit_action:RemoveUpdateCallback()
                                behit_action:Stop()
                                actor:SetProperty(PROP_COMBAT_POS,last_x,last_y)
                                actor:SetDir(last_dir)
                                CurrentCmd:RemoveActing(target)
                                combat_system_remove_from_battle(target)
                            end
                        end)
                    else
                        CurrentCmd:RemoveActing(target)
                    end
                end)
                CurrentCmd:AddActing(target)
                target:PushAction(ACTION_BEHIT)
                target:MoveActionToBack()
            end
        end
    end)
    cast_action:AddStopCallback(function()
        CurrentCmd:RemoveActing(actor)
    end)
    CurrentCmd:AddActing(actor)
    actor:PushAction(ACTION_CAST)
    actor:MoveActionToBack()
end


function on_cast_attack(skill, actor)
    skill_on_start(skill, actor)

    local target = actor:GetTarget()
    local tar_x,tar_y = target:GetPos()
    local degree = actor:GetMoveDestAngle(tar_x,tar_y)
    local dir = actor:GetDirByDegree(degree)
    dir = math_dir_8_to_4(dir)
    actor:SetDir(dir)
    target:SetDir(dir)
    target:ReverseDir()
    
    local runto_x, runto_y =  calc_run_to_pos(actor,target)
    actor:SetProperty(PROP_ASM_RUNTO_X, runto_x)
    actor:SetProperty(PROP_ASM_RUNTO_Y, runto_y)
    
    local runto_action = actor:GetAvatar(ACTION_RUNTO)
    runto_action:Reset()
    runto_action:SetLoop(-1)
    runto_action:SetFrameInterval(PERFRAME_TIME)
    runto_action:AddCallback(0,function()
        local runto_x = actor:GetProperty(PROP_ASM_RUNTO_X)
        local runto_y = actor:GetProperty(PROP_ASM_RUNTO_Y)
        actor:MoveOnScreenWithDuration(runto_x,runto_y,runto_action:GetGroupFrameTime()-PERFRAME_TIME,true)
    end)
    actor:PushAction(ACTION_RUNTO)

    local attack_action = actor:GetAvatar(ACTION_ATTACK)
    attack_action:Reset()
    attack_action:SetLoop(-1)
    local avatar_id = actor:GetProperty(PROP_AVATAR_ID)
    local key_frame = action_get_attack_key_frame(avatar_id)
    if key_frame == 0 then
        key_frame = attack_action:GetKeyFrame()
    end
    attack_action:AddFrameCallback(key_frame, function()
        local behit_action = target:GetAvatar(ACTION_BEHIT)
        behit_action:Reset()
        behit_action:SetLoop(-1)
        behit_action:AddFrameCallback(1, function()
            local avatar = target:GetAvatar()
            local resid = target:GetProperty(PROP_ASM_BEHIT_ANIM)
            local pack, was = res_decode(resid)
            local anim = animation_create(pack,was)
            anim:SetLoop(-1)
            local offy =  -avatar:GetFrameKeyY() + avatar:GetFrameHeight() / 2.0
            anim:SetOffsetY(offy)  
            target:AddFrontAnim(anim)

            local damage = target:GetProperty(PROP_ASM_DAMAGE) 
            target:ShowBeatNumber(damage)
            attack_action:Pause(math.floor(anim:GetGroupFrameTime()* 1000))
            behit_action:Pause(math.floor(anim:GetGroupFrameTime()* 1000))
        end)
        
        behit_action:AddStopCallback(function()
            skill_on_hit(skill, actor)
            if target:IsDead() then
                behit_action:Reset()
                behit_action:Play()
                behit_action:SetLoop(0)

                local last_x, last_y = target:GetPos()
                local last_dir = target:GetDir()
                local dx, dy = 0,0
                local dir_x ,dir_y = actor:GetAttackVec()
                local fly_x ,fly_y = 0,0 
                behit_action:AddUpdateCallback(function()
                    local actor = target
                
                    local px, py = actor:GetPos()
                    local avatar = actor:GetAvatar()
                    if py - avatar:GetFrameKeyY() <= 0 then
                        dir_y = -dir_y
                    end

                    if py - avatar:GetFrameKeyY() + avatar:GetFrameHeight()  >= 600 then
                        dir_y = -dir_y
                    end
                    
                    if avatar:IsFrameUpdate() then
                        px = px + dir_x * 49
                        py = py + dir_y * 49
                        actor:SetProperty(PROP_COMBAT_POS, px,py)
                    end

                    if avatar:IsGroupEndUpdate() then
                        local dir = actor:GetDir()
                        dir = math_next_dir4(dir)
                        actor:SetDir(dir)
                    end
                    if px - avatar:GetFrameKeyX() < 0 then
                        behit_action:RemoveUpdateCallback()
                        behit_action:Stop()
                        actor:SetProperty(PROP_COMBAT_POS,last_x,last_y)
                        actor:SetDir(last_dir)
                        CurrentCmd:RemoveActing(target)
                        combat_system_remove_from_battle(target)
                    end

                    if px - avatar:GetFrameKeyX() + avatar:GetFrameWidth() >= 800 then
                        behit_action:RemoveUpdateCallback()
                        behit_action:Stop()
                        actor:SetProperty(PROP_COMBAT_POS,last_x,last_y)
                        actor:SetDir(last_dir)
                        CurrentCmd:RemoveActing(target)
                        combat_system_remove_from_battle(target)
                    end
                end)
            else
                CurrentCmd:RemoveActing(target)
            end
        end)
        CurrentCmd:AddActing(target)
        target:PushAction(ACTION_BEHIT)
        target:MoveActionToBack()
    end)
    actor:PushAction(ACTION_ATTACK)

    local runback_action = actor:GetAvatar(ACTION_RUNBACK)
    runback_action:Reset()
    runback_action:SetLoop(-1)
    runback_action:SetFrameInterval(PERFRAME_TIME)
    runback_action:AddCallback(0,function()
        actor:ReverseDir()
        local runto_x = actor:GetProperty(PROP_ASM_RUNTO_X)
        local runto_y = actor:GetProperty(PROP_ASM_RUNTO_Y)
        actor:MoveOnScreenWithDuration(-runto_x,-runto_y,runback_action:GetGroupFrameTime()-PERFRAME_TIME,true)
    end)

    runback_action:AddStopCallback(function()
        actor:ReverseDir()
        CurrentCmd:RemoveActing(actor)     
    end)

    CurrentCmd:AddActing(actor) 
    actor:PushAction(ACTION_RUNBACK)
    actor:MoveActionToBack()
end


function on_cast_skill(skill, actor)
    local target = actor:GetTarget()
    if skill.type == 'atk' then
        local damage = actor:GetAttackDamage(false,false,0,1)
        target:SetProperty(PROP_ASM_DAMAGE,damage) 
        target:ModifyHP(-damage)
        target:SetProperty(PROP_ASM_BEHIT_ANIM, skill.atk_anim)
        on_cast_attack(skill, actor)
    elseif skill.type == 'spell' then
        local player = actor_manager_fetch_local_player()
        for i, target in ipairs(BattleActors) do
            if actor:GetProperty(PROP_TEAM_TYPE) ~= target:GetProperty(PROP_TEAM_TYPE) then
                actor:SetTarget(target)
                break
            end
        end
        on_cast_spell(skill,actor)
    end
end