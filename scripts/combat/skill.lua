
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
            else
                cxlog_info(fun,err)
            end
        end 
    end
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
                target:SetProperty(PROP_ASM_BEHIT_ANIM, skill.templ.atk_anim)
        
                local behit_action = target:GetAvatar(ACTION_BEHIT)
                behit_action:Reset()
                behit_action:SetLoop(1)
                behit_action:AddFrameCallback(1, function()
                    local avatar = target:GetAvatar()
                    local resid = skill.templ.atk_anim
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
                    if skill.templ.SkillOnHit then
                        skill.templ.SkillOnHit(skill, actor, target)
                    end
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
        
                                combat_system_current_cmd():RemoveActing(target)
                                combat_system_remove_from_battle(target)
                            end
        
                            if px - avatar:GetFrameKeyX() + avatar:GetFrameWidth() >= 800 then
                                behit_action:RemoveUpdateCallback()
                                behit_action:Stop()
                                actor:SetProperty(PROP_COMBAT_POS,last_x,last_y)
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


function on_cast_attack(skill, actor)
    if skill.templ.SkillOnStart then
        skill.templ.SkillOnStart(skill, actor)
    end

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
            if skill.templ.SkillOnHit then
                skill.templ.SkillOnHit(skill, actor)
            end
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
                        combat_system_current_cmd():RemoveActing(target)
                        combat_system_remove_from_battle(target)
                    end

                    if px - avatar:GetFrameKeyX() + avatar:GetFrameWidth() >= 800 then
                        behit_action:RemoveUpdateCallback()
                        behit_action:Stop()
                        actor:SetProperty(PROP_COMBAT_POS,last_x,last_y)
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
        combat_system_current_cmd():RemoveActing(actor)     
    end)

    combat_system_current_cmd():AddActing(actor) 
    actor:PushAction(ACTION_RUNBACK)
    actor:MoveActionToBack()
end


function on_cast_skill(skill, actor)
    local target = actor:GetTarget()
    if skill.templ.type == 'atk' then
        local damage = actor:GetAttackDamage(false,false,0,1)
        target:SetProperty(PROP_ASM_DAMAGE,damage) 
        target:ModifyHP(-damage)
        target:SetProperty(PROP_ASM_BEHIT_ANIM, skill.templ.atk_anim)
        on_cast_attack(skill, actor)
    elseif skill.templ.type == 'spell' then
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


local ActorMT = actor_get_metatable()

function ActorMT:CastSkill(skill_id, cur_turn)
    cxlog_info('cast skill',skill_id,cur_turn)
    local skill_tpl = skill_table[skill_id]
    if not skill_tpl then return end
    local skill = SkillMT:new()
    skill.tid = skill_id
    skill.turn = cur_turn
    skill.templ = skill_tpl
    on_cast_skill(skill,self)
end


-- function skill_on_start(skill, actor, target)
--     if skill_table[skill.ID] then
--         if skill_table[skill.ID].SkillOnStart then
--             skill_table[skill.ID].SkillOnStart(skill, actor, target)
--         end
--     end
-- end

-- function skill_on_end(skill, actor, target)
--     if skill_table[skill.ID] then
--         if skill_table[skill.ID].SkillOnEnd then
--             skill_table[skill.ID].SkillOnEnd(skill, actor, target)
--         end
--     end
-- end

-- function skill_on_hit(skill, actor, target)
--     if skill_table[skill.ID] then
--         if skill_table[skill.ID].SkillOnHit then
--             skill_table[skill.ID].SkillOnHit(skill, actor, target)
--         end
--     end
-- end