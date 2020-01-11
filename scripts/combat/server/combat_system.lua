__battles__ = __battles__ or {}

local ActorMT = actor_get_metatable()

function ActorMT:GetBattle()
	local battle_id = self:GetProperty(PROP_COMBAT_BATTLE_ID)
	return __battles__[battle_id]
end

function combat_system_create_battle(atk_actor, def_actor)
	local battle = BattleMT:new()

	if atk_actor:HasTeam() then
		local team = atk_actor:GetTeam()
		for i,mem in ipairs(team:GetMembers()) do
			battle:AddActor(mem, TEAM_TYPE_ATTACKER)
		end
	else
		battle:AddActor(atk_actor,TEAM_TYPE_ATTACKER)
	end

	if def_actor:HasTeam() then
		local team = def_actor:GetTeam()
		for i,mem in ipairs(team:GetMembers()) do
			battle:AddActor(mem, TEAM_TYPE_DEFENDER)
		end
	else
		battle:AddActor(def_actor, TEAM_TYPE_DEFENDER)
	end

	__battles__[battle.id] = battle
	return battle
end

function combat_system_remove_battle(battle_id)
	__battles__[battle.id]  = nil
end


stub[PTO_C2S_COMBAT_START] = function(req)
	local atk = actor_manager_fetch_player_by_id(req.atk)
	local def = actor_manager_fetch_player_by_id(req.def)
	local battle = combat_system_create_battle(atk,def)
	battle:StartBattle()
	local resp = req
	resp.battle = battle:Serialize()
	net_send_message_to_all_players(PTO_S2C_COMBAT_START,cjson.encode(resp))
end

--技能就是战斗指令
--玩家的操作也就是 吃道具 逃跑等 还有就是选技能+目标
--[[
	技能表示在战斗回合里，由某个玩家从待战状态，释放技能，再回到待战状态这一过程的表现。
	技能释放后，会产生对战斗对象的影响，影响后续技能的计算
	技能主要阶段：
		技能开始释放（数值结算，表现计算）
		技能释放过程 
		技能释放结束（）	
	cmd = {
		master 
		target 可为空
		技能id
	}
]]



__skills__ = __skills__ or {}

local ActorMT = actor_get_metatable()
function ActorMT:CastSkill(skill_id)
    local actor = self
    local skill_tpl = skill_table[skill_id]
    if not skill_tpl then return end
    local battle = self:GetBattle()
    if not battle then return end
    cxlog_info('cast skill ',skill_id)
    local skill = SkillMT:new()
    __skills__[skill.id] = skill
    self:SetUsingSkill(skill.id)

    skill.caster_end = false
    skill.target_end = false
    skill.tid = skill_id
    skill.turn = battle.turn
    skill.templ = skill_tpl
    skill.group_kill = skill.templ.group_kill
    skill.combo = skill.templ.combo
    skill.type = skill.templ.type
    
    if skill.type == 'atk' then
        if skill.group_kill > 0 then
			if IsClient() then
                on_cast_group_attack(skill, actor)
            end
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
                local damage = actor:GetAttackDamage(target, false,false,0,1)
                table.insert(skill.atk_damage, damage)
                target:ModifyHP(-damage)
                if target:IsDead() then
                    skill.atk_cnt = i
                    break
                end
            end
			if IsClient() then
                on_cast_attack(skill, actor)
            end
        end
    elseif skill.type == 'spell' then
        local to_self_group = false
        if skill.sub_type == SKILL_SUBTYPE_DEFAULT or skill.sub_type == SKILL_SUBTYPE_SEAL then
            to_self_group = false
        elseif skill.sub_type == SKILL_SUBTYPE_HEAL or skill.sub_type == SKILL_SUBTYPE_AUXI then
            to_self_group = true
        end
        local targets = {}
        for i, actor in ipairs(battle.actors) do
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
		if IsClient() then
			on_cast_spell(skill,actor)
		end
    end
end

function ActorMT:SetUsingSkill(skill_uid)
    self:SetProperty(PROP_USING_SKILL, skill_uid)
end

function ActorMT:GetUsingSkill()
    local id = self:GetProperty(PROP_USING_SKILL)
    return __skills__[id]
end

function ActorMT:EndUsingSkill()
    local id = self:GetProperty(PROP_USING_SKILL)
    __skills__[id] = nil
    self:SetProperty(PROP_USING_SKILL, 0)
end

function ActorMT:IsUsingSkill()
    return self:GetProperty(PROP_IS_USING_SKILL)
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

function on_skill_start_atk(skill)
	--[[
		提前确定单体攻击 还是群体攻击 以及攻击目标
		默认实现就是单体攻击 
		选定目标
	]]
	assert(skill.target, 'atk has no target')

	local perform_cmds = {}
	if skill.group_kill > 0 then
		local atk_targets = battle_get_group_kill_targets(skill.battle, skill.group_kill, skill.target)
		for i,target in ipairs(atk_targets) do
			local perform_cmd = {}
			perform_cmd.type = 'atk'
			perform_cmd.master = skill.master:GetID()
			perform_cmd.target = target:GetID()
			perform_cmd.combo = 0	
			local hp_delta = skill.master:GetAttackDamage(target, false,false,0,1)
			target:ModifyHP(-hp_delta)
			perform_cmd.hp_deltas = {hp_delta}
			if target:IsDead() then
				if target:GetProperty(PROP_ACTOR_TYPE) ~= ACTOR_TYPE_PLAYER then
					skill.battle:RemoveActor(target)
					perform_cmd.target_dead_state = ACTOR_LIFE_FLY
				else
					perform_cmd.target_dead_state = ACTOR_LIFE_DEAD
				end
			else
				perform_cmd.target_life_state = ACTOR_LIFE_ALIVE
			end
			table.insert(perform_cmds, perform_cmd)
		end
	else
		local perform_cmd = {}
		perform_cmd.type = 'atk'
		perform_cmd.master = skill.master:GetID()
		perform_cmd.target = target:GetID()
		perform_cmd.combo  = skill.combo
		perform_cmd.hp_deltas = {}
		for i=1,skill.combo do
			local hp_delta = skill.master:GetAttackDamage(skill.target, false,false,0,1)
			target:ModifyHP(-hp_delta)
			table.insert(perform_cmd.hp_deltas, hp_delta)
			if target:IsDead() then
				perform_cmd.combo  = i
				break
			end
		end
		if target:IsDead() then
			if target:GetProperty(PROP_ACTOR_TYPE) ~= ACTOR_TYPE_PLAYER then
				skill.battle:RemoveActor(target)
				perform_cmd.target_dead_state = ACTOR_LIFE_FLY
			else
				perform_cmd.target_dead_state = ACTOR_LIFE_DEAD
			end
		else
			perform_cmd.target_life_state = ACTOR_LIFE_ALIVE
		end
		table.insert(perform_cmds, perform_cmd)
	end

	skill.perform_cmds = skill.perform_cmds
end

function on_skill_start_spell(skill)

end

function on_skill_start_flee(skill)

end

function on_skill_start_trap(skill)

end

function on_skill_start_def(skill)

end

function on_skill_start(skill)
	skill.target_end = false
	skill.caster_end = false

	skill.SkillOnStart(skill)
	if skill.type == 'atk' then
		on_skill_start_atk(skill)
	elseif skill.type == 'spell' then
		on_skill_start_spell(skill)
	elseif skill.type == 'flee' then
		on_skill_start_flee(skill)
	elseif skill.type == 'trap' then
		on_skill_start_trap(skill)
	elseif skill.type == 'def' then
		on_skill_start_def(skill)
	end
end

function process_using_skill(battle, master_id, target_id, skill_id)
	local master = battle:FindActor(master_id)
	if not master then return end
	local skill = SkillMT:new()
	skill.battle = battle
	skill.tid = skill_id
	skill.master = master

	local target = battle:FindActor(target_id)
	if target then
		skill.target = target
	end
	skill.turn = battle.turn

	skill.templ = skill_table[skill_id]
	skill_init_by_templ(skill, skill.templ)			

	on_skill_start(skill)
	return skill.perform_cmds
end

function handle_turn_commands(battle)
	local perform_cmds = {}
	for i,cmd in ipairs(battle.cmds) do
		local perform_cmd = process_using_skill(battle,cmd.master,cmd.target,cmd.skill_id)
		table.insert(perform_cmds, perform_cmd)
	end

	if #perform_cmds > 0 then
		for i,actor in ipairs(battle.actors) do
			if actor:IsPlayer() then
				local pid = actor:GetID()
				net_send_message(pid, PTO_S2C_COMBAT_EXECUTE, cjson.encode(perform_cmds))
			end
		end
	end
end

function handle_turn_old_commands(battle)
	local perform_cmds = {}
	for i,cmd in ipairs(battle.cmds) do
		local master = actor_manager_fetch_player_by_id(cmd.master)
		local target = actor_manager_fetch_player_by_id(cmd.target)
		master:SetTarget(target)
		master:CastSkill(cmd.skill_id)
	end

	if #battle.cmds > 0 then
		for i,actor in ipairs(battle.actors) do
			if actor:IsPlayer() then
				local pid = actor:GetID()
				local msg = { cmds = battle.cmds} 
				net_send_message(pid, PTO_S2C_COMBAT_EXECUTE, cjson.encode(msg))
			end
		end
	end
end


stub[PTO_C2S_COMBAT_CMD] = function(req)
	local master = actor_manager_fetch_player_by_id(req.master)
	local battle =  master:GetBattle()
	if not battle then return cxlog_info('battle not exist!') end
	if battle.state ~= BATTLE_TURN_STAND_BY then return cxlog_info('battle is not in standby') end
	battle:AddCommand(master, req)

	if battle:CheckStandBy() then
		battle.state = BTTALE_TURN_EXECUTE
		handle_turn_old_commands(battle)

		if battle:CheckEnd() then
			battle:EndBattle()
		else
			battle:NextTurn()
		end
	end
end



stub[PTO_C2S_COMBAT_END_BATTLE] = function(req) 
	
end

function combat_system_battle_on_actor_leave(pid)
	local actor = actor_manager_fetch_player_by_id(pid)
	local battle = actor:GetBattle()
	if battle then
		battle:EndBattle()
	end
end

function combat_system_init()
	init_skills()
	init_buffers()
end