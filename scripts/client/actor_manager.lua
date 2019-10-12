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
    elseif actor_type == ACTOR_TYPE_SUMMON then
        row_pos = pet_tbl[role_id].row_pos
    elseif actor_type == ACTOR_TYPE_NPC then
        row_pos = npc_tbl[role_id].row_pos
    end
    print(actor_type, role_id , row_pos )
    return row_pos-1
end

function actor_manager_init()
    
end

function actor_manager_deinit()
    
end

function actor_manager_add_new(tid)

end

