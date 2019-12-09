
local Battles = {}

function on_battle_start(self)
    for i,actor in ipairs(self.actors) do
        actor:SetProperty(PROP_IS_COMBAT,true)
        actor:SetProperty(PROP_TURN_READY,false)
        actor:SetProperty(PROP_COMBAT_BATTLE_ID,self.id)
    end
    self.state = BATTLE_TURN_STAND_BY
	cxlog_info('BATTLE_TURN_STAND_BY')
end

function on_battle_turn_stand_by(self)
    local ready = true
    for i,actor in ipairs(self.actors) do
        if not actor:GetProperty(PROP_TURN_READY) then
            ready = false
            break
        end
    end
    if ready then
        self.state = BTTALE_TURN_EXECUTE
        for i,actor in ipairs(self.actors) do
            local msg = {}
            msg.pid = actor:GetID()
            -- msg.cmds = self.cmds
            net_send_message(msg.pid, PTO_S2C_COMBAT_EXECUTE, cjson.encode(msg))
        end
        cxlog_info('BTTALE_TURN_EXECUTE')
    end
end

function on_battle_turn_execute(self)
    local atk_all_dead = true
    local def_all_dead = true
    for i,actor in ipairs(self.actors) do
        if not actor:IsDead() then
            if actor:GetProperty(PROP_TEAM_TYPE) == TEAM_TYPE_ATTACKER then
                atk_all_dead = false
            else
                def_all_dead = false
            end
        end
    end
    if atk_all_dead or def_all_dead then
        self.state = BTTALE_END
        cxlog_info('BTTALE_END')
    else
        self.state = BTTALE_TURN_NEXT
        cxlog_info('BTTALE_TURN_NEXT')
    end
end

function on_battle_turn_next(self)
    self.turn = self.turn + 1
    for i,actor in ipairs(self.actors) do
        actor:SetProperty(PROP_TURN_READY,false)
    end
    self.state = BATTLE_TURN_STAND_BY
    cxlog_info('BATTLE_TURN_STAND_BY')
end

function on_battle_end(self)
    cxlog_info('BATTLE_END')
    actor:SetProperty(PROP_IS_COMBAT,false)
    actor:SetProperty(PROP_TURN_READY,false)
    actor:SetProperty(PROP_COMBAT_BATTLE_ID,0)
end

function combat_system_fetch_battle(id)
	for i,battle in ipairs(Battles) do
		if battle.id == id then
			return battle
		end
	end
end

function combat_system_create_battle(atk_actors, dfd_actors)
	local battle = BattleMT:new()
	battle.id = os.time()
	local team_id = os.time()
	for i,actor in ipairs(atk_actors) do
		actor:SetProperty(PROP_COMBAT_BATTLE_ID ,battle.id)
		battle:AddActor(actor,team_id,TEAM_TYPE_ATTACKER)
    end

    team_id = team_id + 1
	for i,actor in ipairs(dfd_actors) do
		actor:SetProperty(PROP_COMBAT_BATTLE_ID ,battle.id)
		battle:AddActor(actor,team_id,TEAM_TYPE_DEFENDER)
	end
	battle:StartBattle()
	table.insert(Battles,battle)
	return battle
end

function combat_system_remove_battle(battle_id)
	
end

function combat_system_update_battle()
	for i,battle in ipairs(Battles) do
		battle:Update()
	end
	for i,battle in ipairs(Battles) do
		if battle.state == BATTLE_END then
			table.remove(Battles, i)
		end
	end
end

stub[PTO_C2S_COMBAT_START] = function(req)
	--[[
		客户端发起一场战斗, PVP / PVE , 先做PVE
		客户端队伍 VS 生成NPC队伍
		队长发起战斗后, 队员接受到 进入战斗cmd 都进入战斗 
		服务器收到发起战斗后, 创建battle 以及交战双方, 然后把进入战斗消息下发给客户端
	]]--
	-- 1V1 单挑 
	local atk = actor_manager_fetch_player_by_id(req.atk)
	local def = actor_manager_fetch_player_by_id(req.def)
	combat_system_create_battle({atk},{def})
	net_send_message_to_all_players(PTO_S2C_COMBAT_START,cjson.encode(req) )
end

stub[PTO_C2S_COMBAT_CMD] = function(req)
	-- local battle_id = actor:GetProperty(PROP_COMBAT_BATTLE_ID)
	-- local battle = combat_system_fetch_battle(battle_id)
	-- if battle then
	-- 	battle:AddCommand(cmd)
	-- 	actor:SetProperty(PROP_TURN_READY, true)
	-- end

	if req.type == 'ATK' then
		-- local cmd = CommandMT:new()
        -- cmd:Init(player)
		-- table.insert(Commands,cmd)
		local master = actor_manager_fetch_player_by_id(req.master)  
		local battle_id = master:GetProperty(PROP_COMBAT_BATTLE_ID)
		local battle = combat_system_fetch_battle(battle_id)	

		if battle then
			-- local cmd = CommandMT:new()
			-- battle:AddCommand(cmd)
			master:SetProperty(PROP_TURN_READY, true)
			local target = actor_manager_fetch_player_by_id(req.target)  
			target:SetProperty(PROP_TURN_READY,true)
		end
        -- auto_ready_other_actors()
	end
end
