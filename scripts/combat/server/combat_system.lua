
__battles__ = __battles__ or {}


local ActorMT = actor_get_metatable()

function ActorMT:GetBattle()
	local battle_id = self:GetProperty(PROP_COMBAT_BATTLE_ID)
	return __battles__[battle_id]
end

function combat_system_fetch_battle_by_id(id)
	for i,battle in ipairs(__battles__) do
		if battle.id == id then
			return battle
		end
	end
end

function combat_system_fetch_battle(actor)
	return actor:GetBattle()
end

function combat_system_current_turn(actor)
	local battle_id = actor:GetProperty(PROP_COMBAT_BATTLE_ID)
	local battle = combat_system_fetch_battle_by_id(battle_id)
    if battle then
        return battle.turn
    else
        return 0
    end
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
		local team = atk_actor:GetTeam()
		for i,mem in ipairs(team:GetMembers()) do
			battle:AddActor(mem, TEAM_TYPE_DEFENDER)
		end
	else
		battle:AddActor(def_actor, TEAM_TYPE_DEFENDER)
	end
	
	battle:StartBattle()
	__battles__[battle.id] = battle
	return battle
end

function combat_system_remove_battle(battle_id)
	__battles__[battle.id]  = nil
end

stub[PTO_C2S_COMBAT_START] = function(req)
	local atk = actor_manager_fetch_player_by_id(req.atk)
	local def = actor_manager_fetch_player_by_id(req.def)
	atk:SetProperty(PROP_HP, atk:GetMaxHP())
	def:SetProperty(PROP_HP, def:GetMaxHP())
	req.atk_hp = atk:GetProperty(PROP_HP)
	req.def_hp = def:GetProperty(PROP_HP)

	combat_system_create_battle(atk,def)
	net_send_message_to_all_players(PTO_S2C_COMBAT_START,cjson.encode(req))
end

stub[PTO_C2S_COMBAT_CMD] = function(req)
	local master = actor_manager_fetch_player_by_id(req.master)  
	local battle =  master:GetBattle()
	if battle then
		battle:AddCommand(master, req)
		if battle:CheckStandBy() then
			battle:ExecuteTurn()
		end
	end
end

function combat_system_battle_on_actor_leave(pid)
	local actor = actor_manager_fetch_player_by_id(pid)
	local battle = actor:GetBattle()
	if battle then
		on_battle_end(battle)
	end
end