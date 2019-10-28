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

local CurrentTurn = 1
local Commands = {}
local BattleState = BATTLE_DEFAULT
local tAttackers = {}
local tDefenders = {}
local skill_template_table  = {}

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
    local target = self.main_target
    
    if skill.type == 'atk' then
        local damage = actor:GetAttackDamage(false,false,0,1)
        target:SetProperty(PROP_ASM_DAMAGE,damage) 
        target:ModifyHP(-damage)
        target:SetProperty(PROP_ASM_BEHIT_ANIM, skill.atk_anim)
        on_cast_attack(actor, target,skill)
    elseif skill.type == 'spell' then
        on_cast_spell(actor,skill)
    end
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
        if not self:IsActing() then
            self.state = COMMAND_STATE_STOP
        end
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
    skill_template_table = content_system_get_table('skill')
    -- cxlog_info('combat_system_init',cjson.encode(skill_template_table))
end

function combat_system_all_actor_exe_func(func)
    for i,actor in ipairs(tAttackers) do
        func(actor)
    end

    for i,actor in ipairs(tDefenders) do
        func(actor)
    end
end

function auto_ready_other_actors()
    combat_system_all_actor_exe_func(function(actor)
        if not actor:IsLocal() then
            actor:SetProperty(PROP_TURN_READY,true)
        end
    end)
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
            local player = actor_manager_fetch_local_player()
            player:SetTarget(actor)
            player:SetProperty(PROP_USING_SKILL, 1)

            local cmd = CommandMT:new()
            cmd:Init(player)
            table.insert(Commands,cmd)

            player:SetProperty(PROP_TURN_READY, true)        
            auto_ready_other_actors()
        end)
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
    local reset_actor = function(actor)
        actor:SetProperty(PROP_ASM_BUFF_ANIM, 0)
        actor:ClearBuffAnim()
        actor:SetActionID(ACTION_IDLE)
        actor:SetProperty(PROP_IS_COMBAT,false)
        actor:SetProperty(PROP_CAN_MOVE,true)
        actor:ClearAction()
        actor:PushAction(ACTION_IDLE)

        actor_reg_event(actor, ACTOR_EV_ON_CLICK, actor_on_click_cb[actor:GetID()])
    end

    for i,actor in ipairs(tDefenders) do
        reset_actor(actor)
    end

    for i,actor in ipairs(tAttackers) do
        reset_actor(actor)
    end
    animation_manager_clear()
    scene_set_combat(false)
end


function combat_system_on_turn_end()

end


function check_battle_end()
    do 
        local all_dead = true
        for i,actor in ipairs(tDefenders) do
            if not actor:IsDead() then
                all_dead = false
            end
        end
        if all_dead then return true end
    end

    do 
        local all_dead = true
        for i,actor in ipairs(tAttackers) do
            if not actor:IsDead() then
                all_dead = false
            end
        end
        if all_dead then return true end
    end

    return false
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
    elseif BattleState == BTTALE_END then
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

    local drawActors = {}
    for i,actor in ipairs(tDefenders) do
        table.insert(drawActors,actor)
    end

    for i,actor in ipairs(tAttackers) do
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
    tAttackers = atk_actors
    tDefenders = dfd_actors
    BattleState = BATTLE_START
end

function combat_system_get_group(actor)
    for i,v in ipairs(tAttackers) do
        if v == actor then
            return tAttackers
        end
    end
    for i,v in ipairs(tDefenders) do
        if v == actor then
            return tDefenders
        end
    end
end


function combat_system_remove_from_battle(actor)
    for i,v in ipairs(tAttackers) do
        if v:GetID() == actor:GetID() then
            table.remove(tAttackers,i)
            -- cxlog_info('combat_system_remove_from_battle','atk',i)
            return 
        end
    end
    for i,v in ipairs(tDefenders) do
        if v:GetID() == actor:GetID() then
            table.remove(tDefenders,i)
            -- cxlog_info('combat_system_remove_from_battle','def',i)
            return 
        end
    end
end


function combat_system_on_acting_end(actor)
    actor:SetProperty(PROP_COMBAT_ACTING,false) 
    if actor:IsDead() then
        combat_system_remove_from_battle(actor)
    end
    -- cxlog_info('actor:SetProperty(PROP_COMBAT_ACTING,false) ',actor:GetProperty(PROP_NAME), actor:GetProperty(PROP_HP),actor:IsDead())
    if #Commands > 0 then
        local cmd = Commands[1]
        if cmd.master:GetProperty(PROP_COMBAT_ACTING) == false then
            -- cmd.state = COMMAND_STATE_STOP
            if #tDefenders == 0 then
                cmd.state = COMMAND_STATE_STOP
            end
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

    for i,actor in ipairs(tDefenders) do
        actor:SetProperty(PROP_TURN_READY,true)
    end
end

function combat_system_imgui_update()
    if BattleState == BATTLE_TURN_STAND_BY then
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

            local cmd = CommandMT:new()
            cmd:Init(enemy)
            table.insert(Commands,cmd)

            enemy:SetProperty(PROP_TURN_READY, true)
        end
    end
	imgui.Begin('Menu',menu_show)


    if imgui.Button('攻击##player') then
        test_local_player_cast(1)
    end  

    if imgui.Button('法术##player') then
        test_local_player_cast(51)
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

    for i,actor in ipairs(tDefenders) do
        local x,y,w,h = actor:GetAvatarRect()
        imgui.SetCursorPos(x,y-10)
        local max_hp = actor:GetMaxHP()
        local hp = actor:GetHP()
        imgui.Text(hp..'/'..max_hp)
    end

    for i,actor in ipairs(tAttackers) do
        local x,y,w,h = actor:GetAvatarRect()
        imgui.SetCursorPos(x,y-10)
        local max_hp = actor:GetMaxHP()
        local hp =  actor:GetHP()
        imgui.Text(hp..'/'..max_hp)
    end
end

function BattleCmdOnCombo(actor)

end

function BattleCmdOnGroupKill(actor)

end

function BattleCmdOnTurnReady(actor)

end

function BattleCmdOnTurnEnd(actor)

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

function on_cast_spell(actor, skill)
    local cast_action = actor:GetAvatar(ACTION_CAST)
    cast_action:Reset()
    cast_action:SetLoop(-1)
    cast_action:AddFrameCallback(cast_action:GetGroupFrameCount()/2,function()
        for i, target in ipairs(tDefenders) do
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
                target:AddStateAnim(anim)

                local damage = target:GetProperty(PROP_ASM_DAMAGE) 
                target:ShowBeatNumber(damage)
                behit_action:Pause(math.floor(anim:GetGroupFrameTime()* 1000))
            end)

            behit_action:AddStopCallback(function()
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
                            
                            

                            combat_system_on_acting_end(target)    
                        end

                        if px - avatar:GetFrameKeyX() + avatar:GetFrameWidth() >= 800 then
                            behit_action:RemoveUpdateCallback()
                            behit_action:Stop()
                            actor:SetProperty(PROP_COMBAT_POS,last_x,last_y)
                            actor:SetDir(last_dir)
                            combat_system_on_acting_end(target)    
                        end
                    end)
                else
                    combat_system_on_acting_end(target)
                end
            end)
            target:PushAction(ACTION_BEHIT)
            target:MoveActionToBack()
        end
    end)
    cast_action:AddStopCallback(function()
        combat_system_on_acting_end(actor)
    end)

    actor:PushAction(ACTION_CAST)
    actor:MoveActionToBack()
end


function on_cast_attack(actor, target)
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
            target:AddStateAnim(anim)

            local damage = target:GetProperty(PROP_ASM_DAMAGE) 
            target:ShowBeatNumber(damage)
            attack_action:Pause(math.floor(anim:GetGroupFrameTime()* 1000))
            behit_action:Pause(math.floor(anim:GetGroupFrameTime()* 1000))
        end)
        behit_action:AddStopCallback(function()
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
                    end

                    if px - avatar:GetFrameKeyX() + avatar:GetFrameWidth() >= 800 then
                        behit_action:RemoveUpdateCallback()
                        behit_action:Stop()
                        actor:SetProperty(PROP_COMBAT_POS,last_x,last_y)
                        actor:SetDir(last_dir)
                        CurrentCmd:RemoveActing(target)
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