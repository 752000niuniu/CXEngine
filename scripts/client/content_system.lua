local content_tables = {}
function content_system_get_table(name)
    return content_tables[name] 
end

function content_system_set_table(name, tbl)
    content_tables[name] = tbl
end

local function read_tsv_index_by_main_key(filename, b_parse_value, main_key)
    local tbl_raw = utils_parse_tsv_file_as_table(fs_get_tsv_path(filename), b_parse_value)
    local tbl = {}
    for i,row in ipairs(tbl_raw) do
        row.row_pos = i
        tbl[row[main_key]] = row

    end
    return tbl
end

function content_system_init()
    content_system_set_table('role', read_tsv_index_by_main_key('avatar_role',false,'ID'))
    content_system_set_table('weapon', read_tsv_index_by_main_key('avatar_weapon',false,'ID'))
    content_system_set_table('npc', read_tsv_index_by_main_key('avatar_npc',false,'ID'))
end
