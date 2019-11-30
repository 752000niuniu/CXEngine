local SKILL_SUBTYPE_DEFAULT = 0
local SKILL_SUBTYPE_SEAL = 1
local SKILL_SUBTYPE_HEAL = 2
local SKILL_SUBTYPE_AUXI = 3


local SkillMT = {}
local skill_table = {}
function SkillMT:new(o)
    o = o or {}
    self.__index = self 
    setmetatable(o, self)
    return o
end

function SkillMT:GetTemplate()
    return skill_table[self.tid] 
end

function SkillMT:GetType()
    return skill_table[self.tid].type
end



local skill_env = {
    __index = _ENV
}

function init_skills()
    local skill_template_tbl = content_system_get_table('skill')
    skill_table = skill_template_tbl
    for id,skill in pairs(skill_table) do
        local dir = vfs_get_luapath('../combat/skill/')
        local path = dir..id..'.lua'
        local f = io.open(path)
        if f then
            local chunk = f:read('a')
            f:close()
            local module = setmetatable({},skill_env)
            local fun,err = load(chunk,'@'..path,'bt', module)
            if fun then
                fun()
                skill.SkillOnStart = module.SkillOnStart
                skill.SkillOnEnd = module.SkillOnEnd
                skill.SkillOnHit = module.SkillOnHit
                skill.SkillOnSpell = module.SkillOnSpell
                skill.SkillOnAfterSpell = module.SkillOnAfterSpell
                
            else
                cxlog_info(fun,err)
            end
        end 
    end

    local anim = animation_create(0,0x1D3FF13C) 
    anim.x = 5
    cxlog_info('init ', anim.x)
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
    if skill.templ.SkillOnStart then
        skill.templ.SkillOnStart(skill, actor)
    end
    local cast_action = actor:GetAvatar(ACTION_CAST)
    cast_action:Reset()
    cast_action:SetLoop(-1)
    cast_action:AddFrameCallback(cast_action:GetGroupFrameCount()/2,function()
        for i=1,skill.spell_cnt do
            local target = skill.spell_actors[i]
            local damage = skill.spell_damage[i]

            if skill.templ.sub_type == SKILL_SUBTYPE_SEAL then
                local resid = skill.atk_anim
                local pack, was = res_decode(resid)
                local anim = animation_create(pack,was)
                anim:SetLoop(-1)
                
                target:AddFrontAnim(anim)
                skill.spell_anim = anim
                if skill.templ.SkillOnSpell then
                    skill.templ.SkillOnSpell(skill, actor, target)
                end
                anim:AddStopCallback(function()
                    if skill.templ.SkillOnAfterSpell then
                        skill.templ.SkillOnAfterSpell(skill, actor, target)
                    end 
                end)
            elseif skill.templ.sub_type==SKILL_SUBTYPE_HEAL then
                local resid = skill.atk_anim
                local pack, was = res_decode(resid)
                local anim = animation_create(pack,was)
                anim:SetLoop(-1)
                anim:SetOffsetY(-20)
                target:AddFrontAnim(anim)
                skill.spell_anim = anim
                if skill.templ.SkillOnSpell then
                    skill.templ.SkillOnSpell(skill, actor, target)
                end
                anim:AddStopCallback(function()
                    target:ShowBeatNumber(-damage)
                    if skill.templ.SkillOnAfterSpell then
                        skill.templ.SkillOnAfterSpell(skill, actor, target)
                    end 
                end)
            else
                local behit_action = target:GetAvatar(ACTION_BEHIT)
                behit_action:Reset()
                behit_action:SetLoop(1)
                behit_action:AddFrameCallback(1, function()
                    local resid = skill.atk_anim
                    local pack, was = res_decode(resid)
                    local anim = animation_create(pack,was)
                    anim:SetLoop(-1)
                    
                    target:AddFrontAnim(anim)
                    target:ShowBeatNumber(-damage)
                    behit_action:Pause(math.floor(anim:GetGroupFrameTime()* 1000))
                    skill.spell_anim = anim
                    if skill.templ.SkillOnSpell then
                        skill.templ.SkillOnSpell(skill, actor, target)
                    end
                    anim:AddStopCallback(function()
                        if skill.templ.SkillOnAfterSpell then
                            skill.templ.SkillOnAfterSpell(skill, actor, target)
                        end 
                    end)
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
                                actor:SetCombatPos(px,py)
                            end
        
                            if avatar:IsGroupEndUpdate() then
                                local dir = actor:GetDir()
                                dir = math_next_dir4(dir)
                                actor:SetDir(dir)
                            end
                            if px - avatar:GetFrameKeyX() < 0 then
                                behit_action:RemoveUpdateCallback()
                                behit_action:Stop()
                                actor:SetCombatPos(last_x,last_y)
                                actor:SetDir(last_dir)
        
                                combat_system_current_cmd():RemoveActing(target)
                                combat_system_remove_from_battle(target)
                            end
        
                            if px - avatar:GetFrameKeyX() + avatar:GetFrameWidth() >= 800 then
                                behit_action:RemoveUpdateCallback()
                                behit_action:Stop()
                                actor:SetCombatPos(last_x,last_y)
                                actor:SetDir(last_dir)
                                combat_system_current_cmd():RemoveActing(target)
                                combat_system_remove_from_battle(target)
                            end
                        end)
                    else
                        combat_system_current_cmd():RemoveActing(target)
                    end
                end)
                combat_system_current_cmd():AddActing(target)
                target:PushAction(ACTION_BEHIT)
                target:MoveActionToBack()
            end
            
        end
    end)
    cast_action:AddStopCallback(function()
        combat_system_current_cmd():RemoveActing(actor)
    end)
    combat_system_current_cmd():AddActing(actor)
    actor:PushAction(ACTION_CAST)
    actor:MoveActionToBack()
end

function attack_get_keyframe(actor)
    local attack_action = actor:GetAvatar(ACTION_ATTACK)
    local avatar_id = actor:GetProperty(PROP_AVATAR_ID)
    local key_frame = action_get_attack_key_frame(avatar_id)
    if key_frame == 0 then
        key_frame = attack_action:GetKeyFrame()
    end
    return key_frame
end

function on_attack_action_callback(attack_action)
    local actor = attack_action.actor 
    local target = attack_action.target 
    local skill  = attack_action.skill 
    skill.atk_counter = skill.atk_counter + 1

    if skill.templ.SkillOnHit then
        skill.templ.SkillOnHit(skill, actor)
    end

    local behit_action = target:GetAvatar(ACTION_BEHIT)
    behit_action:Reset()
    behit_action:SetLoop(-1)
    behit_action:AddFrameCallback(1, function()
        local avatar = target:GetAvatar()
        local pack, was = res_decode(skill.templ.atk_anim)
        local anim = animation_create(pack,was)
        anim:SetLoop(-1)
        local offy =  -avatar:GetFrameKeyY() + avatar:GetFrameHeight() / 2.0
        anim:SetOffsetY(offy)  
        target:AddFrontAnim(anim)

        local damage = skill.atk_damage[skill.atk_counter]
        target:ShowBeatNumber(-damage)

        -- actor:ShowBeatNumber(damage)

        attack_action:Pause(math.floor(anim:GetGroupFrameTime()* 1000))
        behit_action:Pause(math.floor(anim:GetGroupFrameTime()* 1000))

        local dir_x ,dir_y = actor:GetAttackVec()
        target:MoveOnScreenWithDuration(dir_x*24,dir_y*24, anim:GetGroupFrameTime() ,true)
    end)
    
    behit_action:AddStopCallback(function()
        if skill.atk_counter == skill.atk_cnt then
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
                        actor:SetCombatPos(px,py)
                    end
    
                    if avatar:IsGroupEndUpdate() then
                        local dir = actor:GetDir()
                        dir = math_next_dir4(dir)
                        actor:SetDir(dir)
                    end
                    if px - avatar:GetFrameKeyX() < 0 then
                        behit_action:RemoveUpdateCallback()
                        behit_action:Stop()
                        actor:SetCombatPos(last_x,last_y)
                        actor:SetDir(last_dir)
                        combat_system_current_cmd():RemoveActing(target)
                        combat_system_remove_from_battle(target)
                    end
    
                    if px - avatar:GetFrameKeyX() + avatar:GetFrameWidth() >= 800 then
                        behit_action:RemoveUpdateCallback()
                        behit_action:Stop()
                        actor:SetCombatPos(last_x,last_y)
                        actor:SetDir(last_dir)
                        combat_system_current_cmd():RemoveActing(target)
                        combat_system_remove_from_battle(target)
                    end
                end)
            else
                local dir_x ,dir_y = actor:GetAttackVec()
                target:MoveOnScreenWithDuration(-dir_x*24,-dir_y*24,PERFRAME_TIME*2,true)
                combat_system_current_cmd():RemoveActing(target)
            end
        else 
            local dir_x ,dir_y = actor:GetAttackVec()
            target:MoveOnScreenWithDuration(-dir_x*24,-dir_y*24,PERFRAME_TIME*2,true)
        end
    end)
    combat_system_current_cmd():AddActing(target)
    target:PushAction(ACTION_BEHIT)
    target:MoveActionToBack()
end

function on_cast_group_attack(skill, actor)

end

function on_cast_attack(skill, actor)
    cxlog_info('on_cast_attack')
    
    if skill.templ.SkillOnStart then
        skill.templ.SkillOnStart(skill, actor)
    end

    local target = actor:GetTarget()
    actor:FaceTo(target)
    
    local runto_action = actor:GetAvatar(ACTION_RUNTO)
    runto_action:Reset()
    runto_action:SetLoop(-1)
    runto_action:SetFrameInterval(PERFRAME_TIME)
    local runto_x, runto_y = calc_run_to_pos(actor,target)
    runto_action.to_x = runto_x
    runto_action.to_y = runto_y

    runto_action:AddStartCallback(function(anim)
        actor:MoveOnScreenWithDuration(anim.to_x,anim.to_y,anim:GetGroupFrameTime()-PERFRAME_TIME,true)
    end)
    actor:PushAction(ACTION_RUNTO)

    local attack_action = actor:GetAvatar(ACTION_ATTACK)
    attack_action:Reset()
    attack_action:SetLoop(skill.atk_cnt)
    attack_action.actor =  actor
    attack_action.target = target
    attack_action.skill =  skill
    skill.atk_counter = 0
    local key_frame = attack_get_keyframe(actor)
    attack_action:AddFrameCallback(key_frame, on_attack_action_callback)
    attack_action:AddLoopCallback(function(anim, counter)
        if counter < anim.skill.atk_cnt then
            attack_action:AddFrameCallback(key_frame, on_attack_action_callback)
        end
    end)
    actor:PushAction(ACTION_ATTACK)

    local runback_action = actor:GetAvatar(ACTION_RUNBACK)
    runback_action:Reset()
    runback_action:SetLoop(-1)
    runback_action:SetFrameInterval(PERFRAME_TIME)
    runback_action.to_x = runto_x
    runback_action.to_y = runto_y
    runback_action:AddStartCallback(function(anim)
        actor:ReverseDir()
        actor:MoveOnScreenWithDuration(-anim.to_x,-anim.to_y,anim:GetGroupFrameTime()-PERFRAME_TIME,true)
    end)

    runback_action:AddStopCallback(function()
        actor:ReverseDir()
        combat_system_current_cmd():RemoveActing(actor)     
    end)

    combat_system_current_cmd():AddActing(actor) 
    actor:PushAction(ACTION_RUNBACK)
    actor:MoveActionToBack()
end

local ActorMT = actor_get_metatable()
function ActorMT:CastSkill(skill_id)
    local actor = self
    local skill_tpl = skill_table[skill_id]
    if not skill_tpl then return end
    local skill = SkillMT:new()
    skill.tid = skill_id
    skill.turn = combat_system_current_turn()
    skill.templ = skill_tpl
    skill.group_kill = skill.templ.group_kill
    skill.combo = skill.templ.combo
    skill.type = skill.templ.type

    if skill.type == 'atk' then
        if skill.group_kill > 0 then
            on_cast_group_attack(skill, actor)
        else
            skill.atk_cnt = 0
            skill.atk_damage = {}
            if skill.combo > 0 then
                skill.atk_cnt = skill.combo
            else
                skill.atk_cnt = 1
            end
            local target = actor:GetTarget()
            for i=1, skill.atk_cnt do
                local damage = actor:GetAttackDamage(false,false,0,1)
                table.insert(skill.atk_damage, damage)
                target:ModifyHP(-damage)
                if target:IsDead() then
                    skill.atk_cnt = i
                    break
                end
            end
            on_cast_attack(skill, actor)
        end
    elseif skill.type == 'spell' then
        local to_self_group = false
        if skill.sub_type == SKILL_SUBTYPE_DEFAULT or skill.sub_type == SKILL_SUBTYPE_SEAL then
            to_self_group = false
        elseif skill.sub_type == SKILL_SUBTYPE_HEAL or skill.sub_type == SKILL_SUBTYPE_AUXI then
            to_self_group = true
        end
        local targets = {}
        for i, actor in ipairs(BattleActors) do
            if to_self_group then
                if actor:GetProperty(PROP_TEAM_TYPE) == self:GetProperty(PROP_TEAM_TYPE) then
                    table.insert(targets,actor)
                end    
            else
                if actor:GetProperty(PROP_TEAM_TYPE) ~= self:GetProperty(PROP_TEAM_TYPE) then
                    table.insert(targets,actor)
                end    
            end
        end
        table.sort(targets,function(a,b)
            return a:CalcSpeed() > b:CalcSpeed()
        end)
        
        if skill.templ.group_kill > 0 then
            skill.spell_cnt = math.min(#targets,skill.group_kill) 
            skill.spell_actors = { }
            for i=1,#targets do
                local target = targets[i]
                if target:GetID() ~= self:GetTarget():GetID() and #skill.spell_actors < skill.spell_cnt-1 then
                    table.insert(skill.spell_actors, target)
                end
            end
            table.insert(skill.spell_actors,self:GetTarget())
        else
            skill.spell_cnt = 1
            skill.spell_actors = {}
            table.insert(skill.spell_actors,self:GetTarget())
        end
        
        skill.spell_damage = {}
        for i=1,skill.spell_cnt do
            local target = skill.spell_actors[i]
            local damage = actor:GetSpellDamage(target)
            if skill.templ.sub_type == SKILL_SUBTYPE_SEAL then
                damage = 0            
            elseif  skill.templ.sub_type == SKILL_SUBTYPE_HEAL then
                damage = -damage
            end
            table.insert(skill.spell_damage,damage)
            target:ModifyHP(-damage)
        end

        skill.atk_anim = skill.templ.atk_anim
        on_cast_spell(skill,actor)
    end
end