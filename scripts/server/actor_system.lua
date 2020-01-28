__summons__ = __summons__ or {}

function summons_on_load()
    local path = vfs_get_workdir() .. '/res/storage/summon.data'
	local db = read_database_file(path)
	if db then
		for i,v in ipairs(db) do
            local pid = v[tostring(PROP_ID)]
            local actor = actor_manager_create_actor(pid)
            actor:SetProperties(v)
            __summons__[pid] = actor
		end
	end
end

function summons_on_save()
    local summon_infos = {}
    for pid, summon in pairs(__summons__) do
        assert(summon:GetProperty(PROP_ACTOR_TYPE) == ACTOR_TYPE_SUMMON)
        table.insert(summon_infos, summon:GetProperties())
    end
    table.sort(summon_infos, function(a,b) return a[PROP_ID] < b[PROP_ID] end)
    	
	local path = vfs_get_workdir() .. '/res/storage/summon.data'
	local fw = io.open(path,'w')
    if not fw then return end
	fw:write(cjson.encode(summon_infos))
	fw:close()
end


stub[PTO_C2S_CREATE_SUMMON] = function(req)
    local pid = utils_next_uid('npc')
    local actor = actor_manager_create_actor(pid)
    actor:SetProperties(req.props)
    actor:SetProperty(PROP_ID, pid)
    actor:SetProperty(PROP_TEAM_ID, 0)
    actor:SetProperty(PROP_ACTOR_TYPE, ACTOR_TYPE_SUMMON)

    cxlog_info('create summon ' ,actor:GetID(), actor:GetProperty(PROP_AVATAR_ID))
    local owner = actor_manager_fetch_player_by_id(req.owner)
    if owner then
        owner:AddSummon(actor)
    end
    __summons__[pid] = actor


    local summon_infos = {}
    for pid, summon in pairs(__summons__) do
        table.insert(summon_infos, summon:GetProperties())
    end
    
    net_send_message(req.owner, PTO_S2C_FETCH_SUMMON_RESP, 
        cjson.encode(summon_infos))

end

stub[PTO_C2S_FETCH_SUMMON] = function(req)
    local summon_infos = {}
    for pid, summon in pairs(__summons__) do
        table.insert(summon_infos, summon:GetProperties())
    end
    
    net_send_message(req.pid, PTO_S2C_FETCH_SUMMON_RESP, 
        cjson.encode(summon_infos))
end

stub[PTO_C2C_MOVE_TO_POS] = function(req,js)
    local player = actor_manager_fetch_player_by_id(req.pid)
    if not player then return end
    player:SetPos(req.x,req.y)
    net_send_message_to_all_players(PTO_C2C_MOVE_TO_POS,js)
end


stub[PTO_C2C_CHAT] = function(req,js)
    net_send_message_to_all_players(PTO_C2C_CHAT,js)
end

stub[PTO_C2S_CREATE_PLAYER] = function(req, js)
    local props = cjson.decode(js)
    local pid = props[tostring(PROP_ID)]
    local actor = actor_manager_fetch_player_by_id(pid)
    actor:SetProperties(props)
end

local ActorMT = actor_get_metatable()
function ActorMT:AddSummon(summon)
    local uids_str = self:GetProperty(PROP_SUMMON_UIDS) 
    uids_str = '[]'
    local uids = cjson.decode(uids_str) 
    table.insert(uids, summon:GetID())
    summon:SetProperty(PROP_SUMMON_HAS_OWNER, true) 
    summon:SetProperty(PROP_SUMMON_OWNER, self:GetID()) 
    self:SetProperty(PROP_SUMMON_UIDS, cjson.encode(uids))
end

function ActorMT:GetSummon()
    local uids_str = self:GetProperty(PROP_SUMMON_UIDS) 
    local uids = cjson.decode(uids_str) 
    if #uids == 0 then return end
    return __summons__[uids[1]]
end

 