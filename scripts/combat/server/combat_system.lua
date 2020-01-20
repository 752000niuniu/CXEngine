__battles__ = __battles__ or {}

local ActorMT = actor_get_metatable()

function ActorMT:GetBattle()
	local battle_id = self:GetProperty(PROP_COMBAT_BATTLE_ID)
	return __battles__[battle_id]
end

function combat_system_add_team_by_actor(battle, actor, team_type)
	if actor:HasTeam() then
		local team = actor:GetTeam()
		for i,mem in ipairs(team:GetMembers()) do
			battle:AddActor(mem, team_type, i)
		end

		for i,actor in ipairs(battle.actors) do
			if actor:IsPlayer() then
				local summon = actor:GetSummon()
				if summon then
					battle:AddActor(summon, team_type, i+5)
				end
			end
		end
	else
		battle:AddActor(actor,team_type,1)
		if actor:IsPlayer() then
			local summon = actor:GetSummon()
			if summon then
				battle:AddActor(summon, team_type,6)
			end
		end
	end
end

function combat_system_create_battle(atk_actor, def_actor)
	local battle = BattleMT:new()
	combat_system_add_team_by_actor(battle, atk_actor, TEAM_TYPE_ATTACKER)
	combat_system_add_team_by_actor(battle, def_actor, TEAM_TYPE_DEFENDER)
	
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

function handle_turn_commands(battle)
	local send_pids = {}
	for i,actor in ipairs(battle.actors) do
		if actor:IsPlayer() then
			table.insert(send_pids, actor:GetID())
		end
	end

	table.sort(battle.cmds, function(a,b)
		local pa = battle:FindActor(a.master)
		local pb = battle:FindActor(b.master)
		if pa and pb then
			return pa:CalcSpeed() > pb:CalcSpeed()
		else
			return true
		end
	end)
	
	local all_skills = {}
	for i,cmd in ipairs(battle.cmds) do
		local skill_info = process_turn_command(battle,cmd.master,cmd.target,cmd.skill_id)
		if skill_info then
			table.insert(all_skills, skill_info)
		end
	end

	for i, pid in ipairs(send_pids) do
		net_send_message(pid, PTO_S2C_COMBAT_EXECUTE, cjson.encode(all_skills))
	end
end

stub[PTO_C2S_COMBAT_CMD] = function(req)
	local battle = __battles__[req.battle_id]
	if not battle then return cxlog_info('battle not exist!') end
	if battle.state ~= BATTLE_TURN_STAND_BY then return cxlog_info('battle is not in standby') end
	
	local master = battle:FindActor(req.master)
	if master:GetProperty(PROP_TURN_READY) then return end
	battle:AddCommand(master, req)

	if battle:CheckStandBy() then
		battle.state = BTTALE_TURN_EXECUTE
		handle_turn_commands(battle)

		if battle:CheckEnd() then
			battle:EndBattle()
		else
			battle:NextTurn()
		end
	end
end

stub[PTO_C2S_COMBAT_END_BATTLE] = function(req) 
	local battle = __battles__[req.battle_id]
	if not battle then return end
	local actor = battle:FindActor(req.pid)
	
	for i,actor in ipairs(battle.actors) do
		if actor:IsPlayer() then
			local msg = {}
			net_send_message(actor:GetID(), PTO_S2C_COMBAT_END_BATTLE, cjson.encode(msg))
		end
	end
	battle:EndBattle()
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
	init_buffs()
end