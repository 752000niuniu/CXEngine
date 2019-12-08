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

local TEAM_TYPE_ATTACKER = 1
local TEAM_TYPE_DEFENDER = 2

local Battles = {}
local BattleMT = {}
function BattleMT:new(o)
    o = o or {
		actors = {},
		cmds = {},
		state = BATTLE_DEFAULT
	}
    self.__index = self 
    setmetatable(o, self)
    return o
end

function BattleMT:AddActor(actor, team_id, team_type)
	actor:SetProperty(PROP_TEAM_ID, team_id)
	actor:SetProperty(PROP_TEAM_TYPE, team_type)
	table.insert(self.actors,actor)
end

function BattleMT:AddCommand(cmd)
	table.insert(self.cmds, cmd)
end

function BattleMT:StartBattle()
	self.state = BATTLE_START
	self.turn = 0
	cxlog_info('BATTLE_START')
end

function BattleMT:Update()
	if self.state == BATTLE_DEFAULT then
		return 
	elseif self.state == BATTLE_START then
		for i,actor in ipairs(self.actors) do
			actor:SetProperty(PROP_IS_COMBAT,true)
			actor:SetProperty(PROP_TURN_READY,false)
			actor:SetProperty(PROP_COMBAT_BATTLE_ID,self.id)
		end
		self.state = BATTLE_TURN_STAND_BY
		cxlog_info('BATTLE_TURN_STAND_BY')
	elseif self.state == BATTLE_TURN_STAND_BY then
		local ready = true
		for i,actor in ipairs(self.actors) do
			if not actor:GetProperty(PROP_TURN_READY) then
				ready = false
				break
			end
		end
		if ready then
			self.state = BTTALE_TURN_EXECUTE
			cxlog_info('BTTALE_TURN_EXECUTE')
		end
	elseif self.state  == BTTALE_TURN_EXECUTE then
		for i,actor in ipairs(self.actors) do
			local msg = {}
			msg.pid = actor:GetID()
			msg.cmds = self.cmds
			net_send_message(PTO_S2C_COMBAT_EXECUTE, cjson.encode(msg))
		end
		
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
	elseif self.state == BTTALE_TURN_NEXT then
		self.turn = self.turn + 1
		for i,actor in ipairs(self.actors) do
            actor:SetProperty(PROP_TURN_READY,false)
		end
		self.state = BATTLE_TURN_STAND_BY
		cxlog_info('BATTLE_TURN_STAND_BY')
	elseif self.state == BATTLE_END then
		cxlog_info('BATTLE_END')
		actor:SetProperty(PROP_IS_COMBAT,false)
		actor:SetProperty(PROP_TURN_READY,false)
		actor:SetProperty(PROP_COMBAT_BATTLE_ID,0)
	end
end

function combat_system_fetch_battle(id)
	for i,battle in ipairs(Battles) do
		if battle.id == id then
			return battle
		end
	end
end

stub[PTO_C2S_COMBAT_CMD] = function(actor, cmd)
	local battle_id = actor:GetProperty(PROP_COMBAT_BATTLE_ID)
	local battle = combat_system_fetch_battle(battle_id)
	if battle then
		battle:AddCommand(cmd)
		actor:SetProperty(PROP_TURN_READY, true)
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