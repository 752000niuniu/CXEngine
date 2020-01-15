
SkillMT = {}
skill_table = {}
function SkillMT:new(o)
    o = o or {
        id = utils_next_uid('skill')
    }
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
end

function skill_init_by_templ(skill, templ)
    skill.atk_anim = templ.atk_anim
    skill.group_kill = templ.group_kill 	--是否群体攻击
    skill.combo = templ.combo				--是否连击
	skill.type = templ.type				
	skill.SkillOnStart = templ.SkillOnStart
	skill.SkillOnEnd = templ.SkillOnEnd
	skill.SkillOnHit = templ.SkillOnHit
	skill.SkillOnSpell = templ.SkillOnSpell
	skill.SkillOnAfterSpell = templ.SkillOnAfterSpell	
end

function battle_get_group_kill_targets(battle, group_kill, main_target)
	local targets = {}
	if not main_target:IsDead() then
		table.insert(targets, main_target)
	end

	local candidates = {}
    for i,actor in ipairs(battle.actors) do
        local name = actor:GetName()
        local name2 = main_target:GetName()
        local type1 = actor:GetProperty(PROP_TEAM_TYPE)
        local type2 = main_target:GetProperty(PROP_TEAM_TYPE)
        local is_dead = actor:IsDead() 
        local is_id_equal = actor:GetID() ~= main_target:GetID()
		if actor:GetProperty(PROP_TEAM_TYPE) == main_target:GetProperty(PROP_TEAM_TYPE)
			and not actor:IsDead() 
			and actor:GetID() ~= main_target:GetID() then
			table.insert(candidates, actor)
		end
	end

    if #candidates > 0 then
        table.sort(candidates, function(a,b) return a:CalcSpeed() > b:CalcSpeed() end)
        for i=1,group_kill-1 do
            local which = math.random(1,#candidates)
            table.insert(targets, candidates[which])
            table.remove(candidates,which)
        end
    end
	return targets
end

function serialize_skill(skill)
    return ret
end

function deserialize_skill(skill)
    return ret
end

function skill_get_targets(battle, skill)
    if skill.group_kill > 0 then
        return battle_get_group_kill_targets(battle, skill.group_kill, skill.target)
    else
        return { skill.target }
    end
end

-- atk_info.target = target:GetID()
--             atk_info.atk_hp_deltas = {}
            
--             local max_atk_cnt = skill.combo > 0 and skill.combo or 1
--             for i=1, max_atk_cnt do
--                 atk_info.atk_cnt = i
            

function on_attack_action_callback(attack_action)
    if not IsClient() then return end
    local skill = attack_action.skill
    local atk_info = attack_action.atk_info
    atk_info.atk_counter = atk_info.atk_counter + 1

    local master = actor_manager_fetch_player_by_id(skill.master)
    local target = actor_manager_fetch_player_by_id(atk_info.target)

    if skill.SkillOnHit then
        skill.SkillOnHit(skill, master)
    end

    local behit_action = target:GetAvatar(ACTION_BEHIT)
    behit_action:Reset()
    behit_action:SetLoop(-1)
    behit_action:AddFrameCallback(1, function()
        local avatar = target:GetAvatar()
        local pack, was = res_decode(skill.atk_anim)
        local anim = animation_create(pack,was)
        anim:SetLoop(-1)
        local offy =  -avatar:GetFrameKeyY() + avatar:GetFrameHeight() / 2.0
        anim:SetOffsetY(offy)  
        target:AddFrontAnim(anim)

        local damage = atk_info.atk_hp_deltas[atk_info.atk_counter]
        target:ShowBeatNumber(-damage)
        -- actor:ShowBeatNumber(damage)

        attack_action:Pause(math.floor(anim:GetGroupFrameTime()* 1000))
        behit_action:Pause(math.floor(anim:GetGroupFrameTime()* 1000))

        local dir_x ,dir_y = master:GetAttackVec()
        target:MoveOnScreenWithDuration(dir_x*24,dir_y*24, anim:GetGroupFrameTime() ,true)
    end)
    
    behit_action:AddStopCallback(function()
        if atk_info.atk_counter == atk_info.atk_cnt then
            if target:IsDead() then
                behit_action:Reset()
                behit_action:Play()
                behit_action:SetLoop(0)
    
                local last_x, last_y = target:GetPos()
                local last_dir = target:GetDir()
                local dx, dy = 0,0
                local dir_x ,dir_y = master:GetAttackVec()
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
                        skill.target_end = true        
                        combat_system_remove_from_battle(target)
                    end
    
                    if px - avatar:GetFrameKeyX() + avatar:GetFrameWidth() >= 800 then
                        behit_action:RemoveUpdateCallback()
                        behit_action:Stop()
                        actor:SetCombatPos(last_x,last_y)
                        actor:SetDir(last_dir)
                        skill.target_end = true   
                        combat_system_remove_from_battle(target)
                    end
                end)
            else
                local dir_x ,dir_y = master:GetAttackVec()
                target:MoveOnScreenWithDuration(-dir_x*24,-dir_y*24,PERFRAME_TIME*2,true)
                skill.target_end = true   
            end
        else 
            local dir_x ,dir_y = master:GetAttackVec()
            target:MoveOnScreenWithDuration(-dir_x*24,-dir_y*24,PERFRAME_TIME*2,true)
        end
    end)
    target:PushAction(ACTION_BEHIT)
    target:MoveActionToBack()
end



function skill_cast_atk(battle, skill)
    local master = actor_manager_fetch_player_by_id(skill.master)
    if not master then return end
    skill.group_atk_counter = skill.group_atk_counter + 1
    skill.caster_end = false
    skill.target_end = false
    cxlog_info('group_atk_counter',skill.id, skill.group_atk_counter)

    master:ClearAction()
    master:PushAction(ACTION_BATIDLE)
    
    local atk_info = skill.atk_infos[skill.group_atk_counter]
    local target_id = atk_info.target
    local target = actor_manager_fetch_player_by_id(target_id)
    master:SetTarget(target)
    master:FaceTo(target)

    local runto_action = master:GetAvatar(ACTION_RUNTO)
    runto_action:Reset()
    runto_action:SetLoop(-1)
    runto_action:SetFrameInterval(PERFRAME_TIME)
    local runto_x, runto_y = calc_run_to_pos(master,target)
    runto_action.to_x = runto_x
    runto_action.to_y = runto_y

    runto_action:AddStartCallback(function(anim)
        master:MoveOnScreenWithDuration(anim.to_x,anim.to_y,anim:GetGroupFrameTime()-PERFRAME_TIME,true)
    end)
    master:PushAction(ACTION_RUNTO)

    local attack_action = master:GetAvatar(ACTION_ATTACK)
    attack_action:Reset()
    attack_action:SetLoop(atk_info.atk_cnt)
    attack_action.skill = skill
    atk_info.atk_counter = 0
    attack_action.atk_info = atk_info
    
    local key_frame = attack_get_keyframe(master)
    attack_action:AddFrameCallback(key_frame, on_attack_action_callback)
    attack_action:AddLoopCallback(function(anim, counter)
        if counter < atk_info.atk_cnt then
            attack_action:AddFrameCallback(key_frame, on_attack_action_callback)
        end
    end)
    master:PushAction(ACTION_ATTACK)

    local runback_action = master:GetAvatar(ACTION_RUNBACK)
    runback_action:Reset()
    runback_action:SetLoop(-1)
    runback_action:SetFrameInterval(PERFRAME_TIME)
    runback_action.to_x = runto_x
    runback_action.to_y = runto_y
    runback_action:AddStartCallback(function(anim)
        master:ReverseDir()
        master:MoveOnScreenWithDuration(-anim.to_x,-anim.to_y,anim:GetGroupFrameTime()-PERFRAME_TIME,true)
    end)

    runback_action:AddStopCallback(function()
        master:ReverseDir()
        skill.caster_end = true     
        local battle = master:GetBattle()
        if not battle:InBattle(target) then
            skill.target_end = true
        end
    end)

    master:PushAction(ACTION_RUNBACK)
    master:MoveActionToBack()
end

function using_atk_skill(battle, skill)
    if IsServer() then
        local client_skill = {}
        client_skill.id = skill.id
        client_skill.tid = skill.tid
        client_skill.master = skill.master:GetID()
        client_skill.targets = {}
        client_skill.type = 'atk'
        client_skill.atk_infos = {}
        
        local master = skill.master
        local targets = skill_get_targets(battle, skill)
        for i,target in ipairs(targets) do
            table.insert(client_skill.targets, target:GetID())
            
            local atk_info = {}
            atk_info.target = target:GetID()
            atk_info.atk_hp_deltas = {}
            
            local max_atk_cnt = skill.combo > 0 and skill.combo or 1
            for i=1, max_atk_cnt do
                atk_info.atk_cnt = i
                local hp_delta = master:GetAttackDamage(target, false,false,0,1)
                target:ModifyHP(-hp_delta)
                table.insert(atk_info.atk_hp_deltas, hp_delta)
    
                if target:IsDead() then
                    break
                end
            end
            if target:IsDead() then
                if target:GetProperty(PROP_ACTOR_TYPE) ~= ACTOR_TYPE_PLAYER then
                    battle:RemoveActor(target)
                    atk_info.target_life_state = ACTOR_LIFE_DEAD_FLY
                else
                    atk_info.target_life_state = ACTOR_LIFE_DEAD
                end
            else
                atk_info.target_life_state = ACTOR_LIFE_ALIVE
            end
            table.insert(client_skill.atk_infos, atk_info)
        end
        return client_skill
    else
        local master = actor_manager_fetch_player_by_id(skill.master)
        if not master then return end
        
        local skill_templ = skill_table[skill.tid]
        skill_init_by_templ(skill, skill_templ)

        if skill.SkillOnStart then
            skill.SkillOnStart(skill, master)
        end

        if #skill.atk_infos > 0 then
            skill.group_atk_counter = 0
            skill_cast_atk(battle, skill)
        end
    end
end

function using_spell_skill(battle, skill)

end

function using_flee_skill(battle, skill)

end

function using_trap_skill(battle, skill)

end

function using_def_skill(battle, skill)
    
end

function on_using_skill(battle, skill)
    if skill.state ~= SKILL_STATE_DEFAULT then return end
    skill.state = SKILL_STATE_START

    local ret
	if skill.type == 'atk' then
		ret = using_atk_skill(battle, skill)
	elseif skill.type == 'spell' then
		ret = using_spell_skill(battle, skill)
	elseif skill.type == 'flee' then
		ret = using_flee_skill(battle, skill)
	elseif skill.type == 'trap' then
		ret = using_trap_skill(battle, skill)
	elseif skill.type == 'def' then
		ret = using_def_skill(battle, skill)
	end
    return ret
end
