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
        { name = 'prop_actor_type' },
        { name = 'prop_avatar_id' },
        { name = 'prop_weapon_avatar_id' },
        { name = 'prop_name' },
        { name = 'prop_pos' },
        { name = 'prop_summon_atk_qual' },
        { name = 'prop_summon_def_qual' },
        { name = 'prop_summon_health_qual' },
        { name = 'prop_summon_magic_qual' },
        { name = 'prop_summon_speed_qual' },
        { name = 'prop_summon_dodge_qual' },
        { name = 'prop_summon_grow_coef' },
        { name = 'prop_base_health' },
        { name = 'prop_base_magic' },
        { name = 'prop_base_force' },
        { name = 'prop_base_stamina' },
        { name = 'prop_base_agility' },
        { name = 'prop_lv' },
        { name = 'prop_scene_id' },
        { name = 'prop_hp' }
    })

    local actor_tbl = content_system_get_table('actor_template')
    local prop_tbl = {}
    for i,row in ipairs(actor_tbl) do
        prop_tbl[row.name] = row.type
    end
    local npc_props = {}
    for i,npc in ipairs(tbl) do
        local props = {}
        for j,prop_name in ipairs(col_names) do
            local prop_id = prop_name_to_id(prop_name)
            local prop_valstr = npc[prop_name]
            local type = prop_tbl[prop_name]
            local v
            if type == 'bool' then
                v = prop_valstr == 'true'
            elseif type == 'int' or type == 'uint64' then
                v = math.tointeger(prop_valstr)
            elseif type == 'float' then
                v = tonumber(prop_valstr)
            elseif type == 'str' then
                v = prop_valstr
            elseif type == 'vec2' then
                local strx,stry = string.match(prop_valstr,'{(.-),(.-)}')
                v = {}
                table.insert(v, tonumber(strx))  
                table.insert(v, tonumber(stry)) 
            end
            props[prop_id] =  v
        end
        table.insert(npc_props, props)
    end
    return npc_props
end

function content_system_init()
    content_system_set_table('role', read_tsv_index_by_main_key('avatar_role',false,'ID'))
    content_system_set_table('weapon', read_tsv_index_by_main_key('avatar_weapon',false,'ID'))
    content_system_set_table('npc', read_npc_table())
    content_system_set_table('magic', read_magic_table())
    content_system_set_table('maps', read_map_table())
    content_system_set_table('actor_template', read_actor_template())

    content_system_set_table('skill', init_skill_template_table())
    content_system_set_table('buff', init_buff_template_table())

    content_system_set_table('school', init_school_templ_table())
    content_system_set_table('equip', init_equip_templ_table())

    content_system_set_table('npc', init_npc_table())
    
    
end
