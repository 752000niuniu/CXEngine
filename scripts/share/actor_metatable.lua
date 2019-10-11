local ActorMT = actor_get_metatable()


local actor_enum_typename_to_type ={
    bool = PROP_TYPE_BOOL,
    int  = PROP_TYPE_INT,
    uint64 = PROP_TYPE_UINT64,
    float = PROP_TYPE_FLOAT,
    str  = PROP_TYPE_STR,
    vec2 = PROP_TYPE_VEC2
}

function actor_on_reg_props(actor)
    local tbl = content_system_get_table('actor_template')
    for i,row in ipairs(tbl) do
        local reg_type = actor_enum_typename_to_type[row.type]
        local v         
        if row.type == 'bool' then
            v = row.def == 'true'
        elseif row.type == 'int' or row.type == 'uint64' then
            v = math.tointeger(row.def)
        elseif row.type == 'float' then
            v = tonumber(row.def)
        elseif row.type == 'str' then
            v = row.def 
        elseif row.type == 'vec2' then
            local strx,stry = string.match(row.def,'{(.-),(.-)}')
            v = {}
            table.insert(v, tonumber(strx))  
            table.insert(v,tonumber(stry)) 
        end
        if row.type ~= 'vec2' then
            actor:RegProperty(i-1,reg_type,v)
        else
            actor:RegProperty(i-1,reg_type,v[1],v[2])
        end
    end
    -- for i,row in ipairs(tbl) do
    --     cxlog_info(actor:GetProperty(i-1))
    -- end 
end

function ActorMT:GetPos()
    if self:GetProperty(PROP_IS_COMBAT) then
        return self:GetProperty(PROP_COMBAT_POS)
    else
        return self:GetProperty(PROP_POS)
    end
end

function ActorMT:StopMove()
    local x,y = self:GetPos()
    self:MoveTo(x,y)
end

local RACE_HUMAN = 1
local RACE_SPIRIT = 2
local RACE_DEVIL = 3

COEF = {
    [RACE_HUMAN] = {
        health = 5,magic = 3, target = 2, damage = 0.7, defend = 1.5,
    },
    [RACE_DEVIL] = {
        health = 6,magic = 2.5, target = 2.3, damage = 0.8, defend = 1.3,
    },
    [RACE_SPIRIT] = {
        health = 4.5,magic = 3.5, target = 1.7, damage = 0.6, defend = 1.6, 
    },
}

--气血 体质×5＋100 体质×6＋100 体质×4.5＋100
function formula_calc_hp(actor)
    local race = actor:GetProperty(PROP_RACE)
    local hp = actor:GetProperty(PROP_BASE_HEALTH) * COEF[race].health + 100
    return hp
end

--魔法 魔力×3+80 魔力×2.5＋80 魔力×3.5＋80
function formula_calc_mp(actor)
    local race = actor:GetProperty(PROP_RACE)
    local mp = actor:GetProperty(PROP_BASE_MAGIC) * COEF[race].magic + 80
    return mp
end

--命中 力量*2+30 力量*2.3+27 力量*1.7+30
function formula_calc_target(actor)
    local race = actor:GetProperty(PROP_RACE)
    local base_target = race == RACE_DEVIL and 27 or 30
    local target = actor:GetProperty(PROP_BASE_FORCE) * COEF[race].target + base_target
    return target
end

--伤害 力量*0.7+34 力量*0.8+34 力量*0.6+40
function formula_calc_damage(actor)
    local race = actor:GetProperty(PROP_RACE)
    local base_damage = race == RACE_DEVIL and 34 or 40
    local damage = actor:GetProperty(PROP_BASE_FORCE) * COEF[race].damage + base_damage
    return damage
end

--防御 耐力*1.5 耐力*1.3 耐力*1.6
function formula_calc_defend(actor)
    local race = actor:GetProperty(PROP_RACE)
    local defend = actor:GetProperty(PROP_BASE_STAMINA) * COEF[race].defend
    return defend
end

--速度 体质×0.1+力量×0.1+耐力×0.1+敏捷×0.7
function formula_calc_speed(actor)
    local speed = actor:GetProperty(PROP_BASE_HEALTH) * 0.1 
                + actor:GetProperty(PROP_BASE_FORCE) * 0.1 
                + actor:GetProperty(PROP_BASE_STAMINA) * 0.1 
                + actor:GetProperty(PROP_BASE_AGILITY) * 0.7
    return speed
end

--躲避 敏捷*1
function formula_calc_dodge(actor)
    local dodge = actor:GetProperty(PROP_BASE_AGILITY) * 1
    return dodge
end

--灵力 体质×0.3+魔力×0.7+力量×0.4+耐力×0.2
function formula_calc_spiritual(actor)
    local spiritual = actor:GetProperty(PROP_BASE_HEALTH) * 0.3
                    + actor:GetProperty(PROP_BASE_MAGIC) * 0.7
                    + actor:GetProperty(PROP_BASE_FORCE) * 0.4 
                    + actor:GetProperty(PROP_BASE_STAMINA) * 0.2
    return spiritual
end


--气血 ＝体质×成长率×2+体力资质×等级÷1000
function formula_calc_summon_hp()

end

--魔法 ＝魔力×成长率+法力资质×等级÷500
function formula_calc_summon_mp()

end

--攻击 ＝(等级×1.5+力量)×成长率÷3.1+[(攻击资质-480)÷300+力量÷2500]×等级
function formula_calc_summon_atk()

end

--防御 ＝(成长率×0.5-0.2)×耐力+[(防御资质-460)÷450+成长率×0.3]×等级
function formula_calc_summon_defend()

end

--速度 ＝速度资质×敏捷÷1000
function formula_calc_summon_speed()

end

--灵力 ＝(法力资质÷1000+成长率)×等级÷5+魔力×0.7+力量×0.4+体质×0.3+耐力×0.2
function formula_calc_summon_spritual()

end

--成长率1 ＝(气血-体力资质×等级÷1000)÷体质÷2
function formula_calc_summon_growth_1()

end

--成长率2 ＝(魔法-法力资质×等级÷500)÷魔力
function formula_calc_summon_growth_2()

end
