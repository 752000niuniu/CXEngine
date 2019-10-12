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
function formula_calc_targethit(actor)
    local race = actor:GetProperty(PROP_RACE)
    local base_target = race == RACE_DEVIL and 27 or 30
    local target = actor:GetProperty(PROP_BASE_FORCE) * COEF[race].target + base_target
    return target
end

--伤害 力量*0.7+34 力量*0.8+34 力量*0.6+40
function formula_calc_atk(actor)
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
    local race = actor:GetProperty(PROP_RACE)
    local dodge = actor:GetProperty(PROP_BASE_AGILITY) * 1 + 10
    if race == RACE_DEVIL then
        dodge = actor:GetProperty(PROP_BASE_AGILITY) * 1 + 8
    end
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


--气血 ＝体质×成长率×6+体力资质×等级÷1000
function formula_calc_summon_hp(actor)
    local hp = actor:GetProperty(PROP_BASE_HEALTH) * actor:GetProperty(PROP_SUMMON_GROW_COEF) * 6
             + actor:GetProperty(PROP_SUMMON_HEALTH_QUAL) * actor:GetProperty(PROP_LV) / 1000
    return hp
end

--魔法 ＝魔力×成长率x3+法力资质×等级÷500
function formula_calc_summon_mp(actor)
    local mp = actor:GetProperty(PROP_BASE_MAGIC) * actor:GetProperty(PROP_SUMMON_GROW_COEF) * 3
            + actor:GetProperty(PROP_SUMMON_MAGIC_QUAL) * actor:GetProperty(PROP_LV) / 500
    return mp
end

--攻击 ＝力量×成长率 + 攻击资质x等级x7/2000
function formula_calc_summon_atk(actor)
    local lv = actor:GetProperty(PROP_LV)
    local force = actor:GetProperty(PROP_BASE_FORCE) 
    local grow = actor:GetProperty(PROP_SUMMON_GROW_COEF)
    local atk_qual = actor:GetProperty(PROP_SUMMON_ATK_QUAL)
    local atk = force * grow + atk_qual*lv*7/2000
    return atk
end

--防御 ＝ 耐力x成长率x4/3 + 防御资质x等级/433
function formula_calc_summon_defend(actor)
    local lv = actor:GetProperty(PROP_LV)
    local stamina = actor:GetProperty(PROP_BASE_STAMINA) 
    local grow = actor:GetProperty(PROP_SUMMON_GROW_COEF)
    local def_qual = actor:GetProperty(PROP_SUMMON_DEF_QUAL)
    local def = stamina*grow*4/3 + def_qual*lv/433
    return def
end

--速度 ＝速度资质×敏捷÷1000
function formula_calc_summon_speed(actor)
    local agility = actor:GetProperty(PROP_BASE_AGILITY) 
    local speed_qual = actor:GetProperty(PROP_SUMMON_SPEED_QUAL)
    local speed = agility * speed_qual / 1000
    return speed
end

--灵力 ＝等级*(法力资质+1640)*(成长率+1)/7500 +魔力×0.7+力量×0.4+体质×0.3+耐力×0.2
function formula_calc_summon_spritual(actor)
    local lv = actor:GetProperty(PROP_LV)
    local grow = actor:GetProperty(PROP_SUMMON_GROW_COEF)
    local magic_qual = actor:GetProperty(PROP_SUMMON_MAGIC_QUAL)
    
    local magic = actor:GetProperty(PROP_BASE_MAGIC) 
    local force = actor:GetProperty(PROP_BASE_FORCE) 
    local health = actor:GetProperty(PROP_BASE_HEALTH) 
    local stamina = actor:GetProperty(PROP_BASE_STAMINA) 
    
    local spiritual = lv*(magic_qual+1640)*(grow+1)/7500 + magic*0.7 + force*0.4 + health*0.3 + stamina*0.2
    return spiritual
end

function formula_calc_summon_dodge(actor)
end

--成长率1 ＝(气血-体力资质×等级÷1000)÷体质÷2
function formula_calc_summon_growth_1(actor)
    return 0
end

--成长率2 ＝(魔法-法力资质×等级÷500)÷魔力
function formula_calc_summon_growth_2(actor)
    return 0
end

function ActorMT:GetMaxHP()
    local actor_type = self:GetProperty(PROP_ACTOR_TYPE) 
    if actor_type == ACTOR_TYPE_PLAYER then
        return formula_calc_hp(self)
    elseif actor_type == ACTOR_TYPE_SUMMON then
        return formula_calc_summon_hp(self)
    end
    return 0
end

function ActorMT:GetMaxMP()
    local actor_type = self:GetProperty(PROP_ACTOR_TYPE) 
    if actor_type == ACTOR_TYPE_PLAYER then
        return formula_calc_mp(self)
    elseif actor_type == ACTOR_TYPE_SUMMON then
        return formula_calc_summon_mp(self)
    end
    return 0
end

function ActorMT:CalcTargetHit()
    local actor_type = self:GetProperty(PROP_ACTOR_TYPE) 
    if actor_type == ACTOR_TYPE_PLAYER then
        return formula_calc_targethit(self)
    end
    return 0
end

function ActorMT:CalcAttack()
    local actor_type = self:GetProperty(PROP_ACTOR_TYPE) 
    if actor_type == ACTOR_TYPE_PLAYER then
        return formula_calc_atk(self)
    elseif actor_type == ACTOR_TYPE_SUMMON then
        return formula_calc_summon_atk(self)
    end
    return 0
end

function ActorMT:CalcDefend()
    local actor_type = self:GetProperty(PROP_ACTOR_TYPE) 
    if actor_type == ACTOR_TYPE_PLAYER then
        return formula_calc_defend(self)
    elseif actor_type == ACTOR_TYPE_SUMMON then
        return formula_calc_summon_defend(self)
    end
    return 0
end

function ActorMT:CalcSpeed()
    local actor_type = self:GetProperty(PROP_ACTOR_TYPE) 
    if actor_type == ACTOR_TYPE_PLAYER then
        return formula_calc_speed(self)
    elseif actor_type == ACTOR_TYPE_SUMMON then
        return formula_calc_summon_speed(self)
    end
    return 0
end

function ActorMT:CalcSpiritual()
    local actor_type = self:GetProperty(PROP_ACTOR_TYPE) 
    if actor_type == ACTOR_TYPE_PLAYER then
        return formula_calc_spiritual(self)
    elseif actor_type == ACTOR_TYPE_SUMMON then
        return formula_calc_summon_spritual(self)
    end
    return 0
end

function ActorMT:CalcDodge()
    local actor_type = self:GetProperty(PROP_ACTOR_TYPE) 
    if actor_type == ACTOR_TYPE_PLAYER then
        return formula_calc_dodge(self)
    end
    return 0
end

local init_prop = {
    [RACE_HUMAN] = {
        10,10,10,10,10
    },
    [RACE_SPIRIT] = {
        12,5,11,12,10
    },
    [RACE_DEVIL] = {
        12,11,11,8,8
    },
}


function ActorMT:UpdatePropPtsByPlan()
    local planstr = self:GetProperty(PROP_ADD_PROP_PLAN) 
    local plan = utils_string_split(planstr,',')
    for i=1,5 do
        plan[i] = tonumber(plan[i])/5
    end

    local lv = self:GetProperty(PROP_LV) 
    local total = (lv+1) * 5 
    local actor_type = self:GetProperty(PROP_ACTOR_TYPE)
    local race = self:GetProperty(PROP_RACE)
    
    local health = 0 
    local magic = 0 
    local force = 0 
    local stamina = 0 
    local agility = 0 
    if actor_type == ACTOR_TYPE_PLAYER then
        health  = init_prop[race][1] + lv + total * plan[1]
        magic   = init_prop[race][2] + lv + total * plan[2]
        force   = init_prop[race][3] + lv + total * plan[3]
        stamina = init_prop[race][4] + lv + total * plan[4]
        agility = init_prop[race][5] + lv + total * plan[5]
    elseif actor_type == ACTOR_TYPE_SUMMON then
        health  = 20 + lv + total * plan[1]
        magic   = 20 + lv + total * plan[2]
        force   = 20 + lv + total * plan[3]
        stamina = 20 + lv + total * plan[4]
        agility = 20 + lv + total * plan[5]
    end

    self:SetProperty(PROP_BASE_HEALTH, health)
    self:SetProperty(PROP_BASE_MAGIC, magic)
    self:SetProperty(PROP_BASE_FORCE, force)
    self:SetProperty(PROP_BASE_STAMINA , stamina)
    self:SetProperty(PROP_BASE_AGILITY, agility)
end

