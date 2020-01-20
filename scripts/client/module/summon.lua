
__summons__ = __summons__ or {}


stub[PTO_S2C_FETCH_SUMMON_RESP] = function(resp)
    for i, info in ipairs(resp) do
        local pid = info[tostring(PROP_ID)]
        local actor = actor_manager_create_actor(pid)
        actor:SetProperties(info)
        actor:SetProperty(PROP_ID, pid)
        __summons__[pid] = actor
    end
end


local ActorMT = actor_get_metatable()

function ActorMT:GetSummon()
    local uids_str = self:GetProperty(PROP_SUMMON_UIDS) 
    local uids = cjson.decode(uids_str) 
    if #uids == 0 then return end
    return __summons__[uids[1]]
end


