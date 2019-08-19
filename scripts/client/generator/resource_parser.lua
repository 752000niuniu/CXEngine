
luadbg_listen(9527)
    
resource_ini_files = 
{
    ["addon.wdf"]       =   "addon.wdf.ini",
    ["atom.wdf"]        =   "atom.wdf.ini",
    -- ["chat.wdf"]        =   "chat.wdf.ini",
    ["firework.wdf"]    =   "firework.wdf.ini",
    ["goods.wdf"]       =   "goods.wdf.ini",
    ["item.wdf"]        =   "item.wdf.ini",
    ["magic.wdf"]       =   "magic.wdf.ini",
    ["mapani.wdf"]      =   "mapani.wdf.ini",
    -- ["mhimage.wdf"]     =   "mhimage.wdf.ini",
    ["misc.wdf"]        =   "misc.wdf.ini",
    -- ["music.wdf"]       =   "music.wdf.ini",
    -- ["scene.wdf"]       =   "scene.wdf.ini",
    ["shape.wdf"]       =   "shape.wdf.ini",
    -- ["shape.wd1"]       =   "shape.wd1.ini",
    ["shape.wd2"]       =   "shape.wd2.ini",
    ["shape.wd3"]       =   "shape.wd3.ini",
    ["shape.wd4"]       =   "shape.wd4.ini",
    ["shape.wd5"]       =   "shape.wd5.ini",
    ["shape.wd6"]       =   "shape.wd6.ini",
    ["shape.wd7"]       =   "shape.wd7.ini",
    ["smap.wdf"]        =   "smap.wdf.ini",
    -- ["sound.wdf"]       =   "sound.wdf.ini",
    -- ["stock.wdf"]       =   "stock.wdf.ini",
    ["waddon.wdf"]      =   "waddon.wdf.ini",
    ["wzife.wdf"]       =   "wzife.wdf.ini",
    -- ["wzife.wd1"]       =   "wzife.wd1.ini",
}


weapon_affix = 
{
    spear       =   {"枪矛","枪"},
    axe         =   {"斧钺","斧铽"},
    sword       =   {"剑"},
    dbswords    =   {"双短剑","双剑"},
    ribbon      =   {"飘带"},
    claw        =   {"爪刺"},
    fan         =   {"扇","扇子"},
    wand        =   {"魔棒","棒"},
    hammer      =   {"锤"},
    whip        =   {"鞭"},
    ring        =   {"环圈","双环"},
    knife       =   {"刀"},
    staff       =   {"法杖"},
    jewel       =   {"宝珠"},
    bow         =   {"弓弩"},
}
function find_weapon_key(weapon_name)
    for weaponkey,names in pairs(weapon_affix) do
        for i,name in ipairs(names) do
            if weapon_name == name then
                return weaponkey
            end
        end
    end
end

resource_ini_tables = {}
local function handle()
	for k,v in pairs(resource_ini_files) do		
		local path = vfs_makepath('res/tables/'..v)
		local str = utils_parse_tsv_file(path)
		if not str then
			cxlog_info("file "..path..' parse error')
		else
			resource_ini_tables[k] = cjson.decode(str)
			-- cxlog_info('file '..path .. ' success')
			if k == 'shape.wdf' then
				-- cxlog_info(str)
			end
		end
	end
	--parse shape.wdf
	local shape_rows = {}
	for k,v in pairs(resource_ini_tables['shape.wdf']) do
		-- cxlog_info(i,v.ID,v.name)
		table.insert(shape_rows, v)
	end
    table.sort(shape_rows,function(a,b)return a.name < b.name end)
    
    local all_types = {
       '90级武器',
        'BB',
        'NPC',
        '人形',
        '人物',
        '人物阴影',
        '武器',
        '物品'}
    
    local weapon_tsv = {}
    local avatar_role_tsv = {}
    local avatar_pet_tsv = {}
    local avatar_npc_tsv = {}
    local extra_tsv = {}

    for i,row in ipairs(shape_rows) do
        local strs = utils_string_split(row.name,'\\')   
        if strs[1] == '90级武器' then
            local weapon = strs[2]
            local subs = utils_string_split(strs[3],'_')
            local name = subs[1]
            local role = subs[2]
            local action = subs[3]
            local weapon_row = {}
            weapon_row.ID = #weapon_tsv
            weapon_row.pack = 'shape.wdf'
            weapon_row.wasid = row.ID
            weapon_row.level = 90
            weapon_row.type = weapon
            weapon_row.name = name
            weapon_row.role = role
            weapon_row.action = action
            table.insert(weapon_tsv, weapon_row)
        elseif strs[1] == 'BB' then
            local pet_row = {}
            pet_row.ID = #avatar_pet_tsv
            pet_row.pack = 'shape.wdf'
            pet_row.wasid = row.ID
            pet_row.name = strs[2]
            pet_row.action = strs[3]
            table.insert(avatar_pet_tsv,pet_row)
        elseif strs[1] == 'NPC' then
            local npc_row = {}
            npc_row.ID = #avatar_npc_tsv
            npc_row.pack = 'shape.wdf'
            npc_row.wasid = row.ID
            npc_row.name = strs[2]
            npc_row.action = strs[3]
            table.insert(avatar_npc_tsv,npc_row)
        elseif strs[1] =='人形' then
            local extra_row = {}
            extra_row.ID = #extra_tsv
            extra_row.pack = 'shape.wdf'
            extra_row.wasid = row.ID
            extra_row.name = '人形'
            extra_row.action = strs[2]
            table.insert(extra_tsv,extra_row)
        elseif strs[1] == '人物' then
            local subs = utils_string_split(strs[2],'_')
            local role_row = {}
            role_row.ID = #avatar_role_tsv
            role_row.pack = 'shape.wdf'
            role_row.wasid = row.ID
            role_row.name = subs[1]
            role_row.action = subs[2]
            role_row.weapon = subs[3] or ''
            table.insert(avatar_role_tsv,role_row)

        elseif strs[1] == '人物阴影' then
            local extra_row = {}
            extra_row.ID = #extra_tsv
            extra_row.pack = 'shape.wdf'
            extra_row.wasid = row.ID
            extra_row.name = '人物阴影'
            table.insert(extra_tsv,extra_row)
        elseif strs[1] == '武器' then
            local subs = utils_string_split(strs[2],'_')
            local weapon_row = {}
            weapon_row.ID = #weapon_tsv
            weapon_row.pack = 'shape.wdf'
            weapon_row.wasid = row.ID
            weapon_row.level = 0
            weapon_row.type = subs[1]
            weapon_row.name = subs[2]..'_'..subs[1]
            weapon_row.role = subs[2]
            weapon_row.action = subs[3]
            table.insert(weapon_tsv,weapon_row)
        elseif strs[1] == '物品' then
             local extra_row = {}
            extra_row.ID = #extra_tsv
            extra_row.pack = 'shape.wdf'
            extra_row.wasid = row.ID
            extra_row.name = strs[2]
            table.insert(extra_tsv,extra_row)
        end        
    end

    local function write_tsv_file(path, tsv)
        local f = io.open(path, 'w')
        for i,row in ipairs(tsv) do
            if i == 1 then
                for k,v in pairs(row) do
                    f:write(k..'\t')        
                end
                f:write('\n')
            end
            for k,v in pairs(row) do
                f:write(v..'\t')        
            end
            f:write('\n')
        end
        f:close()
    end
	write_tsv_file(vfs_makepath('role.tsv'), avatar_role_tsv)
	write_tsv_file(vfs_makepath('weapon.tsv'), weapon_tsv)
	write_tsv_file(vfs_makepath('extra.tsv'), extra_tsv)
	write_tsv_file(vfs_makepath('npc.tsv'), avatar_npc_tsv)
	write_tsv_file(vfs_makepath('pet.tsv'), avatar_pet_tsv)
end

handle()