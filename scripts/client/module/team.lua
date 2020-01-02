

__teams__ = __teams__ or {}



local ActorMT = actor_get_metatable()

function ActorMT:HasTeam()
	local team_id = self:GetProperty(PROP_TEAM_ID)
	return team_id ~= 0
end

function ActorMT:GetTeam()
   	local team_id = self:GetProperty(PROP_TEAM_ID)
    return __teams__[team_id]
end

function ActorMT:IsTeamLeader()
    local team = self:GetTeam()
    if not team then return false end
    return team.leader == self:GetID() 
end

local stub = net_manager_stub()
stub[PTO_S2C_FETCH_TEAM] = function(resp)
	__teams__ = {}	
	for i,team_info in ipairs(resp) do
		__teams__[team_info.id] = team_info
	end
end

stub[PTO_S2C_TEAM_CREATE] = function(resp)
	local team = resp.team
	__teams__[team.id] = team
end

function ActorMT:CreateTeam()
	local req = {}
	req.pid = self:GetID()
	net_send_message(PTO_C2S_TEAM_CREATE, cjson.encode(req))
end

stub[PTO_S2C_TEAM_DISMISS] = function(resp)
	__teams__[resp.team_id] = nil
end

stub[PTO_S2C_TEAM_ADD_MEMBER] = function(resp)
	__teams__[resp.team.id] = resp.team
end

stub[PTO_S2C_TEAM_REMOVE_MEMBER] = function(resp)
	__teams__[resp.team.id] = resp.team
end

function ActorMT:DismissTeam()
	local req = {}
	req.pid = self:GetID()
	net_send_message(PTO_C2S_TEAM_DISMISS, cjson.encode(req))
end

function ActorMT:AddTeamMember(actor)
	local team = self:GetTeam()
	if not team then return end
	local req = {}
	req.team_id = team.id
	req.member_id = actor:GetID()
	net_send_message(PTO_C2S_TEAM_ADD_MEMBER, cjson.encode(req))
end

function ActorMT:RemoveTeamMember(actor)
	local req = {}
	req.team_id = team.id
	req.member_id = actor:GetID()
	net_send_message(PTO_C2S_TEAM_REMOVE_MEMBER, cjson.encode(req))
end