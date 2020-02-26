local content_tables = {}
function content_system_get_table(name)
    return content_tables[name] 
end

function content_system_set_table(name, tbl)
    content_tables[name] = tbl
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
    local tbl  = utils_parse_tsv(vfs_get_tsvpath('avatar_npc'),{
        { name = 'ID'},
        { name = 'can_take', fmt='i', def =0},
        { name = 'idle' },
        { name = 'walk' },
        { name = 'sit' },
        { name = 'angry' },
        { name = 'sayhi' },
        { name = 'dance' },
        { name = 'salute' },
        { name = 'clps' },
        { name = 'cry' },
        { name = 'batidle' },
        { name = 'attack' },
        { name = 'cast' },
        { name = 'behit' },
        { name = 'runto' },
        { name = 'runback' },
        { name = 'defend' },
        { name = 'unknown' }
    })    

    local ret = {}
    for i,row in ipairs(tbl) do
        ret[row.ID] = row
    end
    return ret
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
        { name='sound'},
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

function init_summon_quality_table()
    local tbl,col_names = utils_parse_tsv(vfs_get_tsvpath('summon_quality'),{
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
        { name = 'bgm' },
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


function init_summon_sound() 
    local tbl,col_names = utils_parse_tsv(vfs_get_tsvpath('sound_summon'),{
        { name = 'ID' } ,
        { name = 'clps' } ,
        { name = 'attack' } ,
        { name = 'cast' } ,
        { name = 'behit' } ,
        { name = 'defend' } ,
    })
    local ret = {}
    for i, row in ipairs(tbl) do
        ret[row.ID] = row
    end
    return ret
end

function init_role_sound() 
    local tbl,col_names = utils_parse_tsv(vfs_get_tsvpath('sound_role'),{
        { name = 'ID' },
        { name = 'clps' },
        { name = 'attack' },
        { name = 'cast' },
        { name = 'behit' },
        { name = 'defend' },
    })
    local ret = {}
    for i, row in ipairs(tbl) do
        ret[row.ID] = row
    end
    return ret
end

function read_avatar_role()
    local tbl,col_names = perf_call('1', utils_parse_tsv, vfs_get_tsvpath('avatar_role'),{
        { name = 'ID' } ,
        { name = 'name' } ,
        { name = 'role' } ,
        { name = 'weapon_type' } ,
        { name = 'idle' } ,
        { name = 'walk' } ,
        { name = 'sit' } ,
        { name = 'angry' } ,
        { name = 'sayhi' } ,
        { name = 'dance' } ,
        { name = 'salute' } ,
        { name = 'clps' } ,
        { name = 'cry' } ,
        { name = 'batidle' } ,
        { name = 'attack' } ,
        { name = 'cast' } ,
        { name = 'behit' } ,
        { name = 'runto' } ,
        { name = 'runback' } ,
        { name = 'defend' } ,
        { name = 'unknown' } ,
        { name = 'atk_key', fmt='i', def=0} ,
    })
    local ret = {}
    for i, row in ipairs(tbl) do
        ret[row.ID] = row
    end
    return ret
end

function read_avatar_weapon()
    return utils_parse_tsv_by_main_key(vfs_get_tsvpath('avatar_weapon'),{
        { name = 'ID' } ,
        { name = 'name' } ,
        { name = 'type' } ,
        { name = 'role' } ,
        { name = 'level' } ,
        { name = 'idle' } ,
        { name = 'walk' } ,
        { name = 'sit' } ,
        { name = 'angry' } ,
        { name = 'sayhi' } ,
        { name = 'dance' } ,
        { name = 'salute' } ,
        { name = 'clps' } ,
        { name = 'cry' } ,
        { name = 'batidle' } ,
        { name = 'attack' } ,
        { name = 'cast' } ,
        { name = 'behit' } ,
        { name = 'runto' } ,
        { name = 'runback' } ,
        { name = 'defend' } ,
        { name = 'unknown' } ,
    },'ID')
end

function read_avatar_scene_npc_table()
    return utils_parse_tsv_by_main_key(vfs_get_tsvpath('avatar_scene_npc'),{
        { name = 'ID' } ,
        { name = 'idle' } ,
        { name = 'walk' } ,
        { name = 'sit' } ,
    },'ID')
end

function read_avatar_summon_table()
    return utils_parse_tsv_by_main_key(vfs_get_tsvpath('avatar_summon'),{
        { name = 'ID' },
        { name = 'idle' },
        { name = 'walk' },
        { name = 'clps' },
        { name = 'batidle' },
        { name = 'attack' },
        { name = 'cast' },
        { name = 'behit' },
        { name = 'runto' },
        { name = 'runback' },
        { name = 'defend' },
        { name = 'unknown' },
    }, 'ID')
end

function perf_call(name, f,...)
    local now = time_now()
    local ret = table.pack(f(...))
    local dt = time_now() - now
    cxlog_info(string.format('%s:\t%dms',name,dt))
    return table.unpack(ret)
end

function content_system_init()
    cxlog_info('content_system_init')
    
    content_system_set_table('avatar_role', perf_call('read_avatar_role', read_avatar_role) )
    content_system_set_table('avatar_weapon', perf_call('read_avatar_weapon', read_avatar_weapon) )
    content_system_set_table('avatar_npc', perf_call('read_npc_table', read_npc_table) )
    content_system_set_table('avatar_scene_npc', perf_call('read_avatar_scene_npc_table', read_avatar_scene_npc_table) )
    content_system_set_table('avatar_summon', perf_call('read_avatar_summon_table', read_avatar_summon_table) )
    content_system_set_table('magic', perf_call('read_magic_table', read_magic_table) )
    content_system_set_table('maps', perf_call('read_map_table', read_map_table) )
    content_system_set_table('actor_template', perf_call('read_actor_template', read_actor_template) )
    content_system_set_table('skill', perf_call('init_skill_template_table', init_skill_template_table) )
    content_system_set_table('buff', perf_call('init_buff_template_table', init_buff_template_table) )
    content_system_set_table('school', perf_call('init_school_templ_table', init_school_templ_table) )
    content_system_set_table('equip', perf_call('init_equip_templ_table', init_equip_templ_table) )
    content_system_set_table('npc', perf_call('init_npc_table', init_npc_table) )
    content_system_set_table('summon_quality', perf_call('init_summon_quality_table', init_summon_quality_table) )
    content_system_set_table('scene', perf_call('init_scene_table', init_scene_table) )
    content_system_set_table('transport', perf_call('init_transport_table', init_transport_table) )
    content_system_set_table('summon_sound', perf_call('init_summon_sound', init_summon_sound) )
    content_system_set_table('role_sound', perf_call('init_role_sound', init_role_sound) )
    
end
