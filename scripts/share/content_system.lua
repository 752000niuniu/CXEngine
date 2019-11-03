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

function read_magic_table()
    -- cxlog_info('read_magic_table')
    local file = io.open(vfs_get_tablepath('magic.tsv') )
    local tbl = {}
    local col_names = {}
    local index = 1
    for line in file:lines() do
        -- cxlog_info('line..' ..line)
        if index == 1 then
            col_names = utils_string_split(line,'\t')
        elseif line[0]~='*' and line ~='' then
            local strs = utils_string_split(line,'\t')
            local row = {}
            row.ID = math.tointeger(strs[1]) 
            row.name = strs[2]
            local strs2 = utils_string_split(strs[3],'-') 
            local pack = tonumber(strs2[1],10)
            local wasid = tonumber(strs2[2],16)
            row.resid = (pack<<32)|wasid
            table.insert(tbl,row)
        end
        index = index+1
    end
    file:close()

    local tbl_by_name = {}
    for i,v in ipairs(tbl) do
        tbl_by_name[v.name] = v
    end
    return tbl_by_name
end

function read_map_table()
    local path = vfs_get_tsvpath('map')
    local tbl,col_names = utils_parse_tsv_to_rows(path)
    for i, row in ipairs(tbl) do
        row.ID = math.tointeger(math.ID)
    end
    return tbl
end
function read_npc_table()
    local path = vfs_get_tsvpath('avatar_npc')
    local tbl,col_names = utils_parse_tsv_to_rows(path)
    return tbl
end

function read_actor_template()
    local path = vfs_get_tsvpath('actor_template')
    local tbl,col_names = utils_parse_tsv_to_rows(path)
    return tbl
end


function init_skill_template_table()
    local tbl  = utils_parse_tsv(vfs_get_tsvpath('skill'),{
        { name='ID', fmt='i'},
        { name='name'},
        { name='type'},
        { name='sub_type',fmt='i',def=0},
        { name='combo', fmt='i', def=0},
        { name='atk_anim', fmt=res_parse_resid, def=0},
        { name='group_kill', fmt='i', def=0},
        { name='cast_anim', fmt='i', def=0},
        { name='act_turn', fmt='i', def=0}
    })

    local ret = {}
    for i,row in ipairs(tbl) do
        ret[row.ID] = row
    end
    return ret
end

function init_buffer_template_table()
    local tbl  = utils_parse_tsv(vfs_get_tsvpath('buffer'),{
        { name='ID', fmt='i'},
        { name='name'},
        { name='type',def=0},
        { name='buffer_anim', fmt=res_parse_resid, def=0},
    })

    local ret = {}
    for i,row in ipairs(tbl) do
        ret[row.ID] = row
    end
    return ret
end


function content_system_init()
    content_system_set_table('role', read_tsv_index_by_main_key('avatar_role',false,'ID'))
    content_system_set_table('weapon', read_tsv_index_by_main_key('avatar_weapon',false,'ID'))
    content_system_set_table('npc', read_npc_table())
    content_system_set_table('magic', read_magic_table())
    content_system_set_table('maps', read_map_table())
    content_system_set_table('actor_template', read_actor_template())

    content_system_set_table('skill', init_skill_template_table())
    content_system_set_table('buffer', init_buffer_template_table())
    
end
