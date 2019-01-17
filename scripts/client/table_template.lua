local cjson = require("cjson")

local OpenMode =
{
    ["in"] = 0x1,
    out = 0x2,
    ate = 0x4,
    app = 0x8,
    trunc = 0x10
}
    
resource_ini_files = 
{
    ["addon.wdf"]       =   "addon.wdf.ini",
    ["atom.wdf"]        =   "atom.wdf.ini",
    ["chat.wdf"]        =   "chat.wdf.ini",
    ["firework.wdf"]    =   "firework.wdf.ini",
    ["goods.wdf"]       =   "goods.wdf.ini",
    ["item.wdf"]        =   "item.wdf.ini",
    ["magic.wdf"]       =   "magic.wdf.ini",
    ["mapani.wdf"]      =   "mapani.wdf.ini",
    ["mhimage.wdf"]     =   "mhimage.wdf.ini",
    ["misc.wdf"]        =   "misc.wdf.ini",
    ["music.wdf"]       =   "music.wdf.ini",
    ["scene.wdf"]       =   "scene.wdf.ini",
    ["shape.wdf"]       =   "shape.wdf.ini",
    ["shape.wd1"]       =   "shape.wd1.ini",
    ["shape.wd2"]       =   "shape.wd2.ini",
    ["shape.wd3"]       =   "shape.wd3.ini",
    ["shape.wd4"]       =   "shape.wd4.ini",
    ["shape.wd5"]       =   "shape.wd5.ini",
    ["shape.wd6"]       =   "shape.wd6.ini",
    ["shape.wd7"]       =   "shape.wd7.ini",
    ["smap.wdf"]        =   "smap.wdf.ini",
    ["sound.wdf"]       =   "sound.wdf.ini",
    ["stock.wdf"]       =   "stock.wdf.ini",
    ["waddon.wdf"]      =   "waddon.wdf.ini",
    ["wzife.wdf"]       =   "wzife.wdf.ini",
    ["wzife.wd1"]       =   "wzife.wd1.ini",
}

resource_ini_tables = {}
function init_table_templates()
    for k,v in pairs(resource_ini_files) do
        if v ~= "" then
            log_print("file:"..v)
            local str = utils_parse_tsv_file(v)
            log_print(str)
            resource_ini_tables[k] = cjson.decode(str)
        end
    end
end


function resave_tsv_file_by_name()
    for k,v in pairs(resource_ini_files) do
        if v == "" then
            log_print("file:"..v)
            utils_resave_tsv_file(v)
            -- break
        end
    end
end

avatar_category_affix =
{
    Weapon  =   "武器",
    BB      =   "BB",
    NPC     =   "NPC",
    RenXing =   "人形",
    Role    =   "人物",
}

role_affix =
{
    FYN    =   "飞燕女",
    YNX    =   "英女侠",
    XYS    =   "逍遥生",
    JXK    =   "剑侠客",
    HMR    =   "狐美人",
    GJL    =   "骨精灵",
    JMW    =   "巨魔王",
    HTG    =   "虎头怪",
    WTJ    =   "舞天姬",
    XCE    =   "玄彩娥",
    STB    =   "神天兵",
    LTZ    =   "龙太子",
}

action_affix = 
{
	idle      	=	{"静立"},
	walk      	=	{"行走"},
	sit       	=	{"坐下"},
	angry     	=	{"愤怒","发怒"},
	sayhi     	=	{"招呼","挥手"},
	dance     	=	{"舞蹈","跳舞"},
	salute    	=	{"行礼","鞠躬"},
	clps      	=	{"倒地"},
	cry       	=	{"哭泣"},
	batidle   	=	{"待战","待站"},
	attack    	=	{"攻击"},
	cast      	=	{"施法"},
	behit     	=	{"被击中"},
	runto     	=	{"跑去","跑动"},
	runback   	=	{"跑回","返回"},
	defend    	=	{"防御"},
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

local role_weapon_config = 
{
    ["飞燕女"]  = { "ring"  , "dbswords" },
    ["英女侠"]  = { "dbswords"  , "whip" },
    ["逍遥生"]  = { "sword"  , "fan" },
    ["剑侠客"]  = { "knife"  , "sword" },
    ["狐美人"]  = { "whip"  , "claw" },
    ["骨精灵"]  = { "claw"  , "wand" },
    ["巨魔王"]  = { "axe"  , "knife" },
    ["虎头怪"]  = { "hammer"  , "axe" },
    ["舞天姬"]  = { "ribbon"  , "ring" },
    ["玄彩娥"]  = { "wand"  , "ribbon" },
    ["神天兵"]  = { "spear"  , "hammer" },
    ["龙太子"]  = { "fan"  , "spear" },
}



pack_id_table = 
{
    AddonWDF       =   0x10,
    AtomWDF        =   0x20,
    ChatWDF        =   0x30,
    FireworkWDF    =   0x40,
    GoodsWDF       =   0x50,
    ItemWDF        =   0x60,
    MagicWDF       =   0x70,
    MapaniWDF      =   0x80,
    MhimageWDF     =   0x90,
    MiscWDF        =   0xa0,
    MusicWDF       =   0xb0,
    SceneWDF       =   0xc0,
    ShapeWDF       =   0xd0,
    ShapeWD1       =   0xd1,
    ShapeWD2       =   0xd2,
    ShapeWD3       =   0xd3,
    ShapeWD4       =   0xd4,
    ShapeWD5       =   0xd5,
    ShapeWD6       =   0xd6,
    ShapeWD7       =   0xd7,
    SmapWDF        =   0xe0,
    SoundWDF       =   0xf0,
    StockWDF       =   0x100,
    WaddonWDF      =   0x110,
    WzifeWDF       =   0x120,
    WzifeWD1       =   0x121,
}
        
shape_BB = {"白熊","超级泡泡","大蝙蝠","大海龟","地狱战神","赌徒","风伯","凤凰","芙蓉仙子","古代瑞兽|古代责兽","龟丞相","蛤蟆怪","海毛虫","黑山老妖","黑熊","黑熊精","狐狸精","蝴蝶仙子","护卫","花妖","僵尸","蛟龙","巨蛙","骷髅怪","狼","老虎","雷鸟人","马面","牛头","牛妖","泡泡","强盗","如意仙子","山贼","树怪","天兵","天将","兔子怪","虾兵","小龙女","蟹将","星灵仙子","巡游天神","羊头怪","野鬼","野猪","雨师","蜘蛛精",}
shape_NPC = {"帮派守护兽","捕鱼人","超级巫医","翠花","大大王","大生","大唐","道士","道童","地藏王","赌霸天","二宝","二大王","服装店老板","观音姐姐","管家","国子监祭酒","和尚","红娘","机关人","蒋大全","结婚1","结婚2","九头精怪","酒楼老板","考官","兰虎","老花农","老马猴","老太婆","老太爷","雷黑子","李世民","栗栗娘","龙王","龙王","鲁成","罗百万","罗师爷","马货商","男","牛魔王","女","盘丝","判官","菩提祖师","钱庄老板","秦琼","穷汉","三大王","沙僧","商会总管","少女","生肖狗","生肖猴","生肖虎","生肖鸡","生肖龙","生肖马","生肖牛","生肖蛇","生肖鼠","生肖兔","生肖羊","生肖猪","书生","书童","苏梦梦","孙婆婆","孙悟空","唐僧","特产商人","天宫","铁匠","土地","万圣公主","文老伯","无常白1","无常白2","无常黑1","无常黑2","武器店老板","小白龙","小二","衙役","阎罗王","药店老板","驿站老板","佣人","御林军","月老","张老财","赵姨娘","珍品商人","镇塔之神","镇元大仙","郑镖头","至尊宝","钟馗","猪八戒","装备收购商",}
shape_90_weapon={"暗夜","八卦","百花","碧波","碧波","彩虹","沧海","吹雪","毒牙","鬼牙","红莲","雷神","冷月","梨花","灵蛇","流云","龙筋","盘龙","霹雳","破魄","乾坤","秋风","如意","撕天","肃魂","太极","屠龙","无敌","血刃","胭脂","倚天","阴阳","鱼肠","玉龙","月光","湛卢",}


function find_avatar_table_row(av_table, tag)
    if not av_table[tag] then 
        av_table[tag] = 
        {
            idle	= "",
            walk	= "",
            sit	    = "",
            angry	= "",
            sayhi	= "",
            dance	= "",
            salute	= "",
            clps	= "",
            cry 	= "",
            batidle	= "",
            attack	= "",
            cast	= "",
            behit	= "",
            runto	= "",
            runback	= "",
            defend	= "",
        }
    end
    return av_table[tag]
end

function find_action_key(action_name)
    for action_key,names in pairs(action_affix) do
        for i,name in ipairs(names) do
            if action_name == name then
                return action_key
            end
        end
    end
end

function find_weapon_key(weapon_name)
    for weaponkey,names in pairs(weapon_affix) do
        for i,name in ipairs(names) do
            if weapon_name == name then
                return weaponkey
            end
        end
    end
end

local avatar_weapon = {}
function _parse_90_weapon_row(strs,WASID)
    if #strs ~= 5 then return false end
    local _90weapon = strs[1]
    local _type = strs[2]
    local _name = strs[3]
    local _role = strs[4]
    local _action = strs[5]
    local ID = "Weapon_".._name.."_".._role
    local row = find_avatar_table_row(avatar_weapon,ID)
    row.ID = ID
    row.name = _name
    row.type = find_weapon_key(_type)
    row.role = _role
    row.level = '90'

    local action_key = find_action_key(_action)
    if action_key then
        row[action_key] = WASID
        return true
    end
    return false
end


-- 9B6684AA	武器\刀_剑侠客_被击中
local avatar_weapon_table = {}
function _parse_weapon_row(strs,WASID)
    if #strs ~= 4 then return false end
    local _weapon = strs[2]
    local weaponkey = find_weapon_key(_weapon)
    if not weaponkey then return false end
    
    local _role = strs[3]
    local _action = strs[4]

    local weapon1 = weaponkey.."_0"
    local weapon2 = weaponkey.."_30"
    local weapon3 = weaponkey.."_60"
    local ID1 = _role .. "_" .. weapon1
    local ID2 = _role .. "_" .. weapon2
    local ID3 = _role .. "_" .. weapon3
    
    local row1 = find_avatar_table_row(avatar_weapon_table,ID1)        
    local row2 = find_avatar_table_row(avatar_weapon_table,ID2)        
    local row3 = find_avatar_table_row(avatar_weapon_table,ID3)        

    row1.ID = ID1
    row1.name = ID1
    row1.type = weaponkey
    row1.role = _role
    row1.level = '0'
    
    row2.ID = ID2
    row2.name = ID2
    row2.type = weaponkey
    row2.role = _role
    row2.level = '30'

    row3.ID = ID3
    row3.name = ID3
    row3.type = weaponkey
    row3.role = _role
    row3.level = '60'

    local action_key = find_action_key(_action)
    if action_key then
        if row1[action_key]  == "" then
            row1[action_key] = WASID    
            row2[action_key] = WASID
            row3[action_key] = WASID
        end

        if WASID ~=  row1[action_key] then
            row2[action_key] = WASID
            row3[action_key] = WASID
        end

        if WASID ~=  row2[action_key] then
            row3[action_key] = WASID
        end
        return true
    end
end

-- 922E6CCB	人物\骨精灵_攻击_魔棒
--人物\骨精灵_倒地
local avatar_role_table = {}
function _parse_role_row(strs,WASID)
    if #strs < 3 then return false end
    local _name = strs[2]
    local _action = strs[3]
    if #strs == 3 then
        local weapon1 = role_weapon_config[_name][1]
        local weapon2 = role_weapon_config[_name][2]
        local ID1 = _name .. "_" .. weapon1
        local ID2 = _name .. "_" .. weapon2
        local row1 = find_avatar_table_row(avatar_role_table,ID1)        
        local row2 = find_avatar_table_row(avatar_role_table,ID2)        

        row1.ID = ID1
        row1.name = _name
        row1.weapon = weapon1

        row2.ID = ID2
        row2.name = _name
        row2.weapon = weapon2

        local action_key = find_action_key(_action)
        if action_key then
            if row1[action_key] == "" then
                row1[action_key] = WASID    
                row2[action_key] = WASID    
            end

            if WASID ~=  row1[action_key] then
                row2[action_key] = WASID
            end
            return true
        end

    elseif #strs == 4 then
        local _weapon = strs[4]
        local weapon = find_weapon_key(_weapon)
        if weapon then
            local ID = _name .. "_" .. weapon
            local row = find_avatar_table_row(avatar_role_table,ID)        
            row.ID = ID
            row.name = _name
            row.weapon = weapon
            local action_key = find_action_key(_action)
            if action_key then
                row[action_key] = WASID
                return true    
            end
        else 
            return false
        end
    else 
        return false
    end
end

-- 0B5E6653	BB\凤凰\施法
local avatar_bb_table = {}
function _parse_bb_row(strs,WASID)
    if #strs ~= 3 then return false end
    local _name = strs[2]
    local _action = strs[3]
    local row = find_avatar_table_row(avatar_bb_table,"BB_".._name)
    row.name = _name
    row.ID = "BB_".._name
    local action_key = find_action_key(_action)
    if action_key then
        row[action_key] = WASID
        return true    
    end
    return false
end

-- 3101E857	NPC\万圣公主\行走
local avatar_npc_table = {}
function _parse_npc_row(strs,WASID)
    if #strs ~= 3 then return false end
    local _name = strs[2]
    local _action = strs[3]
    local row = find_avatar_table_row(avatar_npc_table,"NPC_".._name)
    row.name = _name
    row.ID = "NPC_".._name
    local action_key = find_action_key(_action)
    if action_key then
        row[action_key] = WASID
        return true    
    end
    return false
end

function write_avater_file(path, first_row, av_table)
    local fs = utils_file_open(path,OpenMode["in"]|OpenMode.out|OpenMode.trunc)
    utils_file_write(fs,first_row..'\n') 
    utils_file_write(fs,"*\n")

    local av_cols = utils_str_split(first_row,'\t')
    for k,v in pairs(av_table) do
        for i,key in ipairs(av_cols) do
            if v[key] then
                utils_file_write(fs,v[key].."\t")
            end
        end
        utils_file_write(fs,"\n")
    end
    utils_file_close(fs)
end

function generate_avatar_role_tsv()
    for k,v in pairs(resource_ini_files) do
        if k == "shape.wdf" then
            log_print("file:"..v)
            
            local parsed_tsv = utils_parse_tsv_file_as_table(v,false)
            for i,row in ipairs(parsed_tsv) do
                row.ok = false
                local strs = utils_str_split(row.name,'\\_')
                if strs[1] == '90级武器' then
                    row.ok = _parse_90_weapon_row(strs,row.ID)
                elseif strs[1] == 'BB' then
                    row.ok = _parse_bb_row(strs,row.ID)
                elseif strs[1] == 'NPC' then
                    row.ok = _parse_npc_row(strs,row.ID)
                elseif strs[1] == '人物' then 
                    row.ok = _parse_role_row(strs,row.ID)
                elseif strs[1] == '武器' then 
                    row.ok = _parse_weapon_row(strs,row.ID)
                end
            end

            write_avater_file("resource/tables/avatar_weapon.1.tsv",
                                [[ID	name	type	role	level	idle	walk	sit	angry	sayhi	dance	salute	clps	cry	batidle	attack	cast	behit	runto	runback	defend]],
                                avatar_weapon)

            write_avater_file("resource/tables/avatar_bb.1.tsv",
                                [[ID	name	idle	walk	sit	angry	sayhi	dance	salute	clps	cry	batidle	attack	cast	behit	runto	runback	defend]],
                                avatar_bb_table)

            write_avater_file("resource/tables/avatar_npc.1.tsv",
                                [[ID	name	idle	walk	sit	angry	sayhi	dance	salute	clps	cry	batidle	attack	cast	behit	runto	runback	defend]],
                                avatar_npc_table)

            write_avater_file("resource/tables/avatar_role.1.tsv",
                                [[ID	name	weapon	idle	walk	sit	angry	sayhi	dance	salute	clps	cry	batidle	attack	cast	behit	runto	runback	defend]],
                                avatar_role_table)

            write_avater_file("resource/tables/avatar_weapon.2.tsv",
                                [[ID	name	type	role	level	idle	walk	sit	angry	sayhi	dance	salute	clps	cry	batidle	attack	cast	behit	runto	runback	defend]],
                                avatar_weapon_table)



            fs = utils_file_open("resource/tables/sss.1.tsv",OpenMode["in"]|OpenMode.out|OpenMode.trunc)
            utils_file_write(fs,[[ID	name	ok]])

            utils_file_write(fs,"\n*\n")
            for i,row in ipairs(parsed_tsv) do
                if row.ok == false then
                    utils_file_write(fs,tostring(row.ID).."\t")
                    utils_file_write(fs,row.name.."\t")
                    utils_file_write(fs,tostring(row.ok).."\n")
                end
            end
            utils_file_close(fs)
        end
    end
end

function generate_avatar_weapon_tsv()
end

function generate_avatar_pet_tsv()
end

function generate_avatar_npc_tsv()
end


function search_item_res_id(name)
    
end

function search_shape_res_id(name)

end

function search_magic_res_id(name)

end
