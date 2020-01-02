
if IsServer() then
    script_system_dofile('../combat/server/combat_system.lua')
    script_system_dofile('../combat/server/skill.lua') 
    script_system_dofile('../combat/server/buffer.lua') 
end

if IsClient() then
    script_system_dofile('../combat/client/combat_system.lua')
    script_system_dofile('../combat/client/skill.lua')
    script_system_dofile('../combat/client/buffer.lua')
end
 
script_system_dofile('../combat/skill.lua')
script_system_dofile('../combat/buffer.lua')

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
        id = utils_next_uid('battle'),
		actors = {},
		cmds = {},
        state = BATTLE_DEFAULT,
        turn = 0
	}
    self.__index = self 
    setmetatable(o, self)
    return o
end

function BattleMT:Serialize()
    local info = {}
    info.id = self.id
    info.actors = {}
    for i, actor in ipairs(self.actors) do
        table.insert(info.actors, actor:GetID())
    end
    info.state = self.state
    info.turn = self.turn
    return info
end

function BattleMT:Deserialize(info)
    self.id = info.id
    self.actors = {}
    for i,actor_id in ipairs(info.actors) do
        local actor = actor_manager_fetch_player_by_id(actor_id)
        table.insert(self.actors, actor)
    end
    self.state = info.state
    self.turn = info.turn
    return self
end

function BattleMT:AddActor(actor, team_type)
    actor:SetProperty(PROP_TEAM_TYPE, team_type)
    actor:SetProperty(PROP_COMBAT_BATTLE_ID, self.id)
	table.insert(self.actors,actor)
end

function BattleMT:RemoveActor(actor)
    for i,_actor in ipairs(self.actors) do
        if actor:GetID() == _actor:GetID() then
            actor:SetProperty(PROP_COMBAT_BATTLE_ID, 0)
            actor:SetProperty(PROP_IS_COMBAT, false)
            table.remove(self.actors,i)
            break
        end
    end
end

function BattleMT:GetTurn()
    return self.turn
end

function BattleMT:AddCommand(actor, cmd)
    actor:SetProperty(PROP_TURN_READY, true)
	table.insert(self.cmds, cmd)
end

function BattleMT:StartBattle()
	self.state = BATTLE_START
	self.turn = 0
    if IsServer() then
        for i,actor in ipairs(self.actors) do
            actor:SetProperty(PROP_IS_COMBAT,true)
            actor:SetProperty(PROP_TURN_READY,false)
        end
        self.state = BATTLE_TURN_STAND_BY
        self.player_actors = {}
        self.npc_actors = {}
        for i,actor in ipairs(self.actors) do
            if actor:GetProperty(PROP_ACTOR_TYPE) == ACTOR_TYPE_PLAYER then
                table.insert(self.player_actors, actor)
            else
                actor:SetProperty(PROP_TURN_READY, true)
                table.insert(self.npc_actors, actor)
            end
        end
    else
        scene_set_combat(true)
        self.state = BATTLE_TURN_STAND_BY
        self.player_actors = {}
        self.npc_actors = {}
        for i,actor in ipairs(self.actors) do
            if actor:GetProperty(PROP_ACTOR_TYPE) == ACTOR_TYPE_PLAYER then
                table.insert(self.player_actors, actor)
            else
                actor:SetProperty(PROP_TURN_READY, true)
                table.insert(self.npc_actors, actor)
            end
        end
        on_battle_start(self)
    end
end

function BattleMT:EndBattle()
	self.state = BATTLE_END
    cxlog_info('BATTLE_END')
    if IsServer() then
        for i,actor in ipairs(self.actors) do
            actor:SetProperty(PROP_IS_COMBAT,false)
            actor:SetProperty(PROP_COMBAT_BATTLE_ID,0)
        end    
    else
        for i,actor in ipairs(self.actors) do
            combat_reset_actor(actor)
        end
        
        animation_manager_clear()
        scene_set_combat(false)
        self.state = BATTLE_DEFAULT
    end
end

function BattleMT:NextTurn()
    self.turn = self.turn + 1
    if IsServer() then
        for i,actor in ipairs(self.actors) do
            if actor:GetProperty(PROP_ACTOR_TYPE) ~= ACTOR_TYPE_PLAYER then
                actor:SetProperty(PROP_TURN_READY,true)
            else
                actor:SetProperty(PROP_TURN_READY,false)
            end
            cxlog_info(actor:GetName(), actor:GetProperty(PROP_HP))
        end
    end
     -- for i,actor in ipairs(self.actors) do
    --     actor:BufferNextTurn(self.turn)
    -- end
	self.cmds = {}
    self.state = BATTLE_TURN_STAND_BY
	cxlog_info('BATTLE_TURN_STAND_BY')
end

function BattleMT:ExecuteTurn()
    self.state = BTTALE_TURN_EXECUTE
    if IsServer() then
        for i,cmd in ipairs(self.cmds) do
            local master = actor_manager_fetch_player_by_id(cmd.master)
            local target = actor_manager_fetch_player_by_id(cmd.target)
            master:SetTarget(target)
            master:CastSkill(cmd.skill_id)
            cxlog_info('cmd ', cjson.encode(cmd))
        end
        if #self.cmds > 0 then
            for i,actor in ipairs(self.player_actors) do
                local pid = actor:GetID()
                local msg = { cmds = self.cmds} 
                -- msg.pid = actor:GetID()
                -- msg.cmds = self.cmds
                net_send_message(pid, PTO_S2C_COMBAT_EXECUTE, cjson.encode(msg))
            end
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
            self:EndBattle()
        else
            self.state = BTTALE_TURN_NEXT
            cxlog_info('BTTALE_TURN_NEXT')
            self:NextTurn()
        end
    else

    end
end

function BattleMT:CheckStandBy()
    local ready = true
	for i,actor in ipairs(self.actors) do
        if not actor:GetProperty(PROP_TURN_READY) then
            ready = false
            break
        end
    end
    return ready
end

