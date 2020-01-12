
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
		if actor:GetProperty(PROP_TEAM_TYPE) == main_target:GetProperty(PROP_TEAM_TYPE)
			and not actor:IsDead() 
			and actor:GetID() ~= main_target:GetID() then
			table.insert(candidates, actor)
		end
	end

	table.sort(candidates, function(a,b) return a:CalcSpeed() > b:CalcSpeed() end)
	for i=1,group_kill-1 do
		local which = math.random(1,#candidates)
		table.insert(targets, candidates[which])
		table.remove(candidates,which)
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

        if skill_templ.SkillOnStart then
            skill_templ.SkillOnStart(skill, master)
        end

        for i,atk_info in ipairs(skill.atk_infos) do
            local target_id = atk_info.target
            local target = actor_manager_fetch_player_by_id(target_id)
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
            attack_action:SetLoop(skill.atk_cnt)
            attack_action.master =  master
            attack_action.target = target
            attack_action.skill =  skill
            skill.atk_counter = 0
            local key_frame = attack_get_keyframe(master)
            attack_action:AddFrameCallback(key_frame, on_attack_action_callback)
            attack_action:AddLoopCallback(function(anim, counter)
                if counter < anim.skill.atk_cnt then
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