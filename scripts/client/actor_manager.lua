local tActors = {}
local ACTOR_ID_COUNTER = 1

local actor_table = {}


local function utils_find_role_id_row(actor_type, role_id)
    local role_tbl =  content_system_get_table('role')
    local pet_tbl =  content_system_get_table('pet')
    -- local weapon_tbl =  content_system_get_table('weapon')
    local npc_tbl =  content_system_get_table('npc')

    local row_pos =  0
    if actor_type == ACTOR_TYPE_PLAYER then
        row_pos = role_tbl[role_id].row_pos
    elseif actor_type == ACTOR_TYPE_PET then
        row_pos = pet_tbl[role_id].row_pos
    elseif actor_type == ACTOR_TYPE_NPC then
        row_pos = npc_tbl[role_id].row_pos
    end
    print(actor_type, role_id , row_pos )
    return row_pos-1
end

function actor_manager_init()
    if true then return end
    local tbl_raw = utils_parse_tsv_file_as_table(fs_get_tsv_path('actor_template'), false)
    for i,row in ipairs(tbl_raw) do
        local ID = row.ID
        local name = row.name
        local type = math.tointeger(row.actor_type)
        local role_row_pos = utils_find_role_id_row(type, row.role_id)
        -- local weapon_id = wep math.tointeger(row.weapon_id)
        local actor = lua_new_actor(type,role_row_pos)
        actor:set_pos(50,50*i)
        if i == 2 then
            actor:set_dir(3)
        end
        -- print(ID,name,type,role_row_pos)
        table.insert(tActors,actor)
    end
    
    -- local tbl = {}
    -- local role_tbl =  content_system_get_table('role')
    -- for k, row in pairs(tbl) do
    --     print(k, row.name)
    -- end
end

function actor_manager_deinit()
    
end

function actor_manager_add_new(tid)
    local actor = lua_new_actor(type)

end

