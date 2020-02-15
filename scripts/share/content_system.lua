local content_tables = {}
function content_system_get_table(name)
    return content_tables[name] 
end

function content_system_set_table(name, tbl)
    cxlog_info('content_system_set_table', name)
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
    local tbl  = utils_parse_tsv(vfs_get_tsvpath('actor_template'),{
        { name='name'},
        { name='type'},
        { name='def'},
        { name='sync', fmt='i', def=0},
    })    
    return tbl
end


function init_skill_template_table()
    local tbl  = utils_parse_tsv(vfs_get_tsvpath('skill'),{
        { name='ID', fmt='i'},
        { name='name'},
        { name='type'},
        { name='sub_type',fmt='i',def=0},
        { name='school',fmt='i',def=0},
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

function init_buff_template_table()
    local tbl  = utils_parse_tsv(vfs_get_tsvpath('buff'),{
        { name='ID', fmt='i'},
        { name='name'},
        { name='type',def=0},
        { name='buff_anim', fmt=res_parse_resid, def=0},
        { name='turn', fmt='i', def=0},
    })

    local ret = {}
    for i,row in ipairs(tbl) do
        ret[row.ID] = row
    end
    return ret
end

function init_school_templ_table()
    local tbl = utils_parse_tsv(vfs_get_tsvpath('school'),{
        {name = 'ID' , fmt = 'i'},
        {name = 'name'}
    })
    local ret = {}
    for i,row in ipairs(tbl) do
        ret[row.ID] = row
    end
    return ret
end

function init_equip_templ_table()
    local tbl = utils_parse_tsv(vfs_get_tsvpath('equip'),{
        {name = 'name'},
        {name = 'type'},
        {name = 'base_target', fmt = 'n' , def = 0},
        {name = 'lv_accum_target', fmt = 'n' , def = 0},
        {name = 'base_damage', fmt = 'n' , def = 0},
        {name = 'lv_accum_damage', fmt = 'n' , def = 0},
        {name = 'base_defend', fmt = 'n' , def = 0},
        {name = 'lv_accum_defend', fmt = 'n' , def = 0},
        {name = 'base_hp', fmt = 'n' , def = 0},
        {name = 'lv_accum_hp', fmt = 'n' , def = 0},
        {name = 'base_mp', fmt = 'n' , def = 0},
        {name = 'lv_accum_mp', fmt = 'n' , def = 0},
        {name = 'base_spiritual', fmt = 'n' , def = 0},
        {name = 'lv_accum_spiritual', fmt = 'n' , def = 0},
        {name = 'base_agile', fmt = 'n' , def = 0},
        {name = 'lv_accum_agile', fmt = 'n' , def = 0}
    })
    local ret = {}
    for i,row in ipairs(tbl) do
        ret[row.type] = row
    end
    return ret
end

local function prop_name_to_id(name)
    local v = load(string.format('do return %s end',name:upper()))()
    return v
end

function init_npc_table()
    local tbl,col_names = utils_parse_tsv(vfs_get_tsvpath('npc'),{
        { name = 'ID' ,fmt='i'},
        { name = 'scene_id', fmt='i', def =0  },
        { name = 'name' },
        { name = 'avatar_id' },
        { name = 'pos', fmt='pos' },
        { name = 'dir', fmt='i', def =0 },
    })
    return tbl
end

function init_summon_table()
    local tbl,col_names = utils_parse_tsv(vfs_get_tsvpath('summon'),{
        { name = 'name' },
        { name = 'take_level', fmt='i', def=0},
        { name = 'battle_level', fmt='i',def=0 },
        { name = 'atk_qual', fmt='i' },
        { name = 'def_qual', fmt='i' },
        { name = 'health_qual', fmt='i' },
        { name = 'magic_qual', fmt='i' },
        { name = 'speed_qual', fmt='i' },
        { name = 'dodge_qual', fmt='i' },
        { name = 'grow_coef', fmt='n' },
    })

    local ret = {}
    for i,row in ipairs(tbl) do
        ret[row.name] = row
    end
    return ret
end

function init_scene_table()
    local tbl,col_names = utils_parse_tsv(vfs_get_tsvpath('scene'),{
        { name = 'ID', fmt='i' },
        { name = 'name'},
        { name = 'map_id', fmt='i' },
        { name = 'script'},
        { name = 'birth_pos', fmt='pos' }
    })
    local ret = {}
    for i, row in ipairs(tbl) do
        ret[row.ID] = row
    end
    return ret
end

function init_transport_table()
    local tbl,col_names = utils_parse_tsv(vfs_get_tsvpath('transport'),{
        { name = 'ID', fmt='i' },
        { name = 'name'},
        { name = 'scene', fmt='i' },
        { name = 'pos', fmt='pos'},
        { name = 'to_station', fmt='i',def=1 }
    })
    local ret = {}
    for i, row in ipairs(tbl) do
        ret[row.ID] = row
    end
    return ret
end

function content_system_init()
    cxlog_info('content_system_init')
    
    content_system_set_table('role', read_tsv_index_by_main_key('avatar_role',false,'ID'))
    content_system_set_table('weapon', read_tsv_index_by_main_key('avatar_weapon',false,'ID'))
    content_system_set_table('avatar_npc', read_npc_table())
    content_system_set_table('magic', read_magic_table())
    content_system_set_table('maps', read_map_table())
    content_system_set_table('actor_template', read_actor_template())

    content_system_set_table('skill', init_skill_template_table())
    content_system_set_table('buff', init_buff_template_table())

    content_system_set_table('school', init_school_templ_table())
    content_system_set_table('equip', init_equip_templ_table())

    content_system_set_table('npc', init_npc_table())
    content_system_set_table('summon', init_summon_table())

    content_system_set_table('scene', init_scene_table())

    content_system_set_table('transport', init_transport_table())
    
    
end
