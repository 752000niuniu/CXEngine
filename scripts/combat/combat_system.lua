
PERFRAME_TIME = 0.016*2.5  

BATTLE_DEFAULT = 0
BATTLE_PREPARE = 1
BATTLE_START = 2
BATTLE_TURN_STAND_BY = 3
BTTALE_TURN_EXECUTE = 4
BTTALE_TURN_NEXT = 5
BATTLE_END = 6

COMMAND_STATE_PREPARE = 1
COMMAND_STATE_PALY = 2
COMMAND_STATE_STOP = 3

COMMAND_TYPE_ATTACK = 1
COMMAND_TYPE_CAST = 2

TEAM_TYPE_ATTACKER = 1
TEAM_TYPE_DEFENDER = 2



BattleMT = {}
function BattleMT:new(o)
    o = o or {
		actors = {},
		cmds = {},
        state = BATTLE_DEFAULT,
        turn = 0
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
    cxlog_info('BattleMT:Update', self.state)
    if self.state == BATTLE_DEFAULT then
		return 
    elseif self.state == BATTLE_START then
        on_battle_start(self)	
    elseif self.state == BATTLE_TURN_STAND_BY then
        on_battle_turn_stand_by(self)
    elseif self.state  == BTTALE_TURN_EXECUTE then
        on_battle_turn_execute(self)
    elseif self.state == BTTALE_TURN_NEXT then
        on_battle_turn_next(self)
    elseif self.state == BATTLE_END then
        on_battle_end(self)
	end
end


if IsServer() then
    script_system_dofile('../combat/server/combat_system.lua')
end

if IsClient() then
    script_system_dofile('../combat/client/combat_system.lua')
end
