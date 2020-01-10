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
]]

--[[
	cmd = {
		master 
		target 可为空
		技能id
	}
]]
function process_using_skill(battle, master_id, target_id, skill_id)
	local master = battle:FindActor(master_id)
	if not master then return end
	local target = battle:FindActor(target_id)
	if target then
		master:SetTarget(target)
	end	
	master:CastSkill(skill_id)
end

function handle_turn_commands(battle)
	for i,cmd in ipairs(battle.cmds) do
		process_using_skill(battle,cmd.master,cmd.target,cmd.skill_id)
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