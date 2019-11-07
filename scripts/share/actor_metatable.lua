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


function ActorMT:GetY()
    local x,y = self:GetPos()
    return y
end


function ActorMT:GetX()
    local x,y = self:GetPos()
    return x
end

function ActorMT:StopMove()
    local x,y = self:GetPos()
    self:MoveTo(x,y)
end

function ActorMT:FaceTo(target)
    local actor = self
    local tar_x,tar_y = target:GetPos()
    local degree = actor:GetMoveDestAngle(tar_x,tar_y)
    local dir = actor:GetDirByDegree(degree)
    dir = math_dir_8_to_4(dir)
    actor:SetDir(dir)
    target:SetDir(dir)
    target:ReverseDir()
end


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
    local target_force = actor:GetProperty(PROP_BASE_FORCE) * COEF[race].target + base_target
    return target_force
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
function formula_calc_summon_spiritual(actor)
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

function ActorMT:CalcSchoolSkillTargethit()
    local school = self:GetProperty(PROP_SCHOOL)
    local skill_lv = self:GetProperty(PROP_SCHOOL_SKILL_LV_TARGETHIT)
    local targethit = 0
    for i=1,skill_lv do
        if school == SCHOOL_DT then
            targethit = targethit + 0.97 + i*0.0203
        elseif school == SCHOOL_LB then
            targethit = targethit + 1 + i*0.0099
        elseif school == SCHOOL_PS then
            targethit = targethit + i*3
        end
    end
    return targethit
end

function ActorMT:CalcSchoolSkillDamage()
    local school = self:GetProperty(PROP_SCHOOL)
    local skill_lv = self:GetProperty(PROP_SCHOOL_SKILL_LV_DAMAGE)
    local damage = 0
    for i=1,skill_lv do
        if school == SCHOOL_DT then
            if i == 1 then
                damage = (69 - 46)
            else
                damage = damage + 2.46 + 0.0143*i
            end
        elseif school == SCHOOL_HS then
            if i == 1 then
                damage = (53 - 46)
            else
                damage = damage + 2 + 0.02*i
            end
        elseif school == SCHOOL_FC then
            if i == 1 then
                damage = (63 - 46)
            else
                damage = damage + 1.9799 + 0.02019*i
            end
        elseif school == SCHOOL_NE then
            if i == 1 then
                damage = (73 - 46)
            else
                damage = damage +  2.015+ 0.01*i
            end
        elseif school == SCHOOL_TG then
            if i == 1 then
                damage = (73 - 46)
            else
                damage = damage + 2.4612+ 0.02039*i
            end
        elseif school == SCHOOL_LG then
            if i == 1 then
                damage = (87 - 46)
            else
                damage = damage + 2.0973+ 0.01899*i
            end
        elseif school == SCHOOL_WZ then
            if i == 1 then
                damage = (77 - 46)
            else
                damage = damage + 1.48049+ 0.02009*i
            end
        elseif school == SCHOOL_PT then
            if i == 1 then
                damage = (67 - 46)
            else
                damage = damage + 1.99+ 0.02*i
            end
        elseif school == SCHOOL_ST then
            if i == 1 then
                damage = (80 - 48)
            else
                damage = damage + 1.976+ 0.0091*i
            end
        elseif school == SCHOOL_MW then
            if i == 1 then
                damage = (80 - 48)
            else
                damage = damage + 1.98309+ 0.02019*i
            end
        elseif school == SCHOOL_DF then
            if i == 1 then
                damage = (100 - 48)
            else
                damage = damage + 2.3766+ 0.021073*i --夜晚
                -- damage = damage + 1.46569+ 0.0092*i 白天
            end
        elseif school == SCHOOL_PS then
            if i == 1 then
                damage = (85 - 48)
            else
                damage = damage + 2.5002 + 0.0089*i
            end
        else
            if i == 1 then
                damage = (75 - 48)
            else
               damage = damage  + 2.46 + 0.0143*i
            end
        end        
    end
    return damage
end

function ActorMT:CalcSchoolSkillDefend()
    local school = self:GetProperty(PROP_SCHOOL)
    local skill_lv = self:GetProperty(PROP_SCHOOL_SKILL_LV_DEFEND)
    local defend = 0
    for i=1,skill_lv do
        defend = defend + 1 + 0.0138*i
    end
    return defend
end

function ActorMT:CalcSchoolSkillSpeed()
    local school = self:GetProperty(PROP_SCHOOL)
    if school ~= SCHOOL_NE then return 0 end
    local skill_lv = self:GetProperty(PROP_SCHOOL_SKILL_LV_SPEED)
    local speed = skill_lv/0.7+math.sqrt(skill_lv) - skill_lv/4.2 + skill_lv*skill_lv/5000
    return speed   
end

function ActorMT:CalcSchoolSkillDodge()
    local school = self:GetProperty(PROP_SCHOOL)
    if school == SCHOOL_NE then return 0 end
    local skill_lv = self:GetProperty(PROP_SCHOOL_SKILL_LV_DODGE)
    local dodge = 0
    for i=1,skill_lv do
        if school ~= SCHOOL_NE then
            dodge = dodge + 2.1775  + 0.01839 *i
        end
    end
    return dodge    
end

function ActorMT:CalcSchoolSkillSpiritual()
    local school = self:GetProperty(PROP_SCHOOL)
    local skill_lv = self:GetProperty(PROP_SCHOOL_SKILL_LV_SPIRITUAL)
    local spiritual = 0
    for i=1,skill_lv do
        if school == SCHOOL_PS then
            spiritual = spiritual + 0.46639 + 0.0141*i
        else
            spiritual = spiritual + 0.46639 + 0.0092*i
        end
    end
    return spiritual    
end

function ActorMT:CalcSchoolSkillHP(base_hp)
    local school = self:GetProperty(PROP_SCHOOL)
    if school ~= SCHOOL_TG then return 0 end
    local skill_lv = self:GetProperty(PROP_SCHOOL_SKILL_LV_SPIRITUAL)
    local hp = base_hp
    for i=1,skill_lv do
        hp = hp*(1+0.003)
    end
    hp = hp - base_hp
    return hp
end

function ActorMT:CalcSchoolSkillMP(base_mp)
    local school = self:GetProperty(PROP_SCHOOL)
    if school ~= SCHOOL_WZ then return 0 end
    local skill_lv = self:GetProperty(PROP_SCHOOL_SKILL_LV_SPIRITUAL)
    local mp = base_mp
    for i=1,skill_lv do
        mp = mp*(1+0.005)
    end
    mp = mp - base_mp
    return mp
end


function ActorMT:GetMaxHP()
    local actor_type = self:GetProperty(PROP_ACTOR_TYPE) 
    if actor_type == ACTOR_TYPE_PLAYER then
        local hp  = formula_calc_hp(self)
        local equip_hp = self:GetProperty(PROP_EQUIP_HP)
        local skill_hp = self:CalcSchoolSkillHP(hp+equip_hp)
        return hp + equip_hp + skill_hp
    elseif actor_type == ACTOR_TYPE_SUMMON then
        return formula_calc_summon_hp(self)
    end
    return 0
end

function ActorMT:GetMaxMP()
    local actor_type = self:GetProperty(PROP_ACTOR_TYPE) 
    if actor_type == ACTOR_TYPE_PLAYER then
        local mp = formula_calc_mp(self)
        local equip_mp = self:GetProperty(PROP_EQUIP_MP)
        local skill_mp = self:CalcSchoolSkillMP(equip_mp+ mp)
        return mp + equip_mp+ skill_mp
    elseif actor_type == ACTOR_TYPE_SUMMON then
        return formula_calc_summon_mp(self)
    end
    return 0
end

function ActorMT:GetHP()
    return math.floor(self:GetProperty(PROP_HP))
end

function ActorMT:GetMP()
    return math.floor(self:GetProperty(PROP_MP))
end

function ActorMT:ModifyHP(delta)
    local hp = self:GetProperty(PROP_HP) 
    hp = delta + hp
    if hp <= 0 then 
        hp = 0 
    end
    local max_hp = self:GetMaxHP()
    if hp >= max_hp then
        hp = max_hp
    end
    self:SetProperty(PROP_HP,hp) 
    if hp  <= 0 then
        self:SetProperty(PROP_IS_DEAD, true)
    else
        self:SetProperty(PROP_IS_DEAD, false)
    end
end


function ActorMT:ModifyMP(delta)
    local mp = self:GetProperty(PROP_MP) 
    mp = delta + mp
    if mp <= 0 then 
        mp = 0 
    end
    local max_mp = self:GetMaxHP()
    if mp >= max_mp then
        mp = max_mp
    end
    self:SetProperty(PROP_MP,mp) 
end



function ActorMT:CalcTargetHit()
    local actor_type = self:GetProperty(PROP_ACTOR_TYPE) 
    if actor_type == ACTOR_TYPE_PLAYER then
        local prop_targethit = formula_calc_targethit(self)
        local equip_targethit = self:GetProperty(PROP_EQUIP_TARGET)
        local skill_targethit = self:CalcSchoolSkillTargethit()
        return prop_targethit +equip_targethit + skill_targethit
    end
    return 0
end

function ActorMT:CalcAttack()
    local actor_type = self:GetProperty(PROP_ACTOR_TYPE) 
    if actor_type == ACTOR_TYPE_PLAYER then
        local prop_atk = formula_calc_atk(self)
        local skill_atk = self:CalcSchoolSkillDamage()
        local equip_targethit = self:GetProperty(PROP_EQUIP_TARGET)
        local equip_atk = self:GetProperty(PROP_EQUIP_DAMAGE)
        local skill_targethit = self:CalcSchoolSkillTargethit()
        return prop_atk + skill_atk + equip_atk +(equip_targethit + skill_targethit) /3
    elseif actor_type == ACTOR_TYPE_SUMMON then
        return formula_calc_summon_atk(self)
    end
    return 0
end

function ActorMT:CalcDefend()
    local actor_type = self:GetProperty(PROP_ACTOR_TYPE) 
    if actor_type == ACTOR_TYPE_PLAYER then
        local prop_def = formula_calc_defend(self)
        local skill_def = self:CalcSchoolSkillDefend()
        local equip_defend = self:GetProperty(PROP_EQUIP_DEFEND)
        return prop_def + equip_defend + skill_def
    elseif actor_type == ACTOR_TYPE_SUMMON then
        return formula_calc_summon_defend(self)
    end
    return 0
end

function ActorMT:CalcSpeed()
    local actor_type = self:GetProperty(PROP_ACTOR_TYPE) 
    if actor_type == ACTOR_TYPE_PLAYER then
        local prop_speed = formula_calc_speed(self)
        local equip_aglie = self:GetProperty(PROP_EQUIP_AGILE)
        local  skill_speed = self:CalcSchoolSkillSpeed(self)
        return prop_speed + equip_aglie*0.7+ skill_speed
    elseif actor_type == ACTOR_TYPE_SUMMON then
        return formula_calc_summon_speed(self)
    end
    return 0
end

function ActorMT:CalcSpiritual()
    local actor_type = self:GetProperty(PROP_ACTOR_TYPE) 
    if actor_type == ACTOR_TYPE_PLAYER then
        local prop_spiritual = formula_calc_spiritual(self)
        local skill_spiritual = self:CalcSchoolSkillSpiritual()
        local equip_spiritual = self:GetProperty(PROP_EQUIP_SPIRITUAL)
        return prop_spiritual+ equip_spiritual+ skill_spiritual
    elseif actor_type == ACTOR_TYPE_SUMMON then
        return formula_calc_summon_spiritual(self)
    end
    return 0
end

function ActorMT:CalcDodge()
    local actor_type = self:GetProperty(PROP_ACTOR_TYPE) 
    if actor_type == ACTOR_TYPE_PLAYER then
        local prop_dodge = formula_calc_dodge(self)
        local equip_aglie = self:GetProperty(PROP_EQUIP_AGILE)
        local skill_dodge = self:CalcSchoolSkillDodge()
        return prop_dodge + equip_aglie+ skill_dodge
    end
    return 0
end

function ActorMT:IsDead()
    return self:GetProperty(PROP_HP) <= 0 
end

function ActorMT:GetRaceName()
    local race = self:GetProperty(PROP_RACE)
    if race == RACE_DEVIL then
        return '魔'
    elseif race == RACE_HUMAN then
        return '人'
    elseif race == RACE_SPIRIT then
        return '仙'
    end
end

function ActorMT:GetSchoolName()
    local school_tbl = content_system_get_table('school')
    local school = self:GetProperty(PROP_SCHOOL)
    return school_tbl[school].name
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

function ActorMT:SetPropsByPlan(plan)
    for k,v in pairs(plan) do
        plan[k] = tonumber(v)/5
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
        health  = init_prop[race][1] + lv + total * plan.health
        magic   = init_prop[race][2] + lv + total * plan.magic
        force   = init_prop[race][3] + lv + total * plan.force
        stamina = init_prop[race][4] + lv + total * plan.stamina
        agility = init_prop[race][5] + lv + total * plan.agility
    elseif actor_type == ACTOR_TYPE_SUMMON then
        health  = 20 + lv + total * plan.health
        magic   = 20 + lv + total * plan.magic
        force   = 20 + lv + total * plan.force
        stamina = 20 + lv + total * plan.stamina
        agility = 20 + lv + total * plan.agility
    end

    self:SetProperty(PROP_BASE_HEALTH, health)
    self:SetProperty(PROP_BASE_MAGIC, magic)
    self:SetProperty(PROP_BASE_FORCE, force)
    self:SetProperty(PROP_BASE_STAMINA , stamina)
    self:SetProperty(PROP_BASE_AGILITY, agility)
end

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



    self:SetProperty(PROP_SCHOOL_SKILL_LV_TARGETHIT, lv) 
    self:SetProperty(PROP_SCHOOL_SKILL_LV_DAMAGE, lv) 
    self:SetProperty(PROP_SCHOOL_SKILL_LV_DEFEND, lv) 
    self:SetProperty(PROP_SCHOOL_SKILL_LV_SPEED, lv) 
    self:SetProperty(PROP_SCHOOL_SKILL_LV_DODGE, lv) 
    self:SetProperty(PROP_SCHOOL_SKILL_LV_SPIRITUAL, lv) 
end


-- 伤害基数

--  一般命中：伤害基数＝攻击－防御
--  爆击：伤害基数＝（攻击－防御）×2
--  连击：
--  第一次攻击的 伤害基数＝（75％×攻击－防御）×2
--  第二次攻击的 伤害基数＝（75％×攻击－防御）×2
local ACTOR_TYPE_COEF_PVP = 1
local ACTOR_TYPE_COEF_PVE = 0.9
local ACTOR_TYPE_COEF_EVP = 0.8
local ACTOR_TYPE_COEF_EVE = 1
function formula_calc_atk_base_damage(atk, def, is_critical, is_combo, combo_coef, actor_type_coef)
    
    local base = atk - def
    if is_combo then
        base = combo_coef*atk - def --第一次0.75 第二次0.5
    end

    if is_critical then
        base = base * 2
    end

    base = base * actor_type_coef
    -- cxlog_info('atk',atk,'def',def,'damage', base)
    return base
end

function formula_calc_atk_float_damage(atk, damage)
    if atk * 0.1 <= damage and damage <= atk * 0.9 then
        damage = damage * math.random(90,110)/100
        -- cxlog_info('0.1~0.9', damage, 'atk', atk)
    elseif damage < atk * 0.1 then
        damage = atk * math.random(10,20)/100
        -- cxlog_info('<0.1',damage , 'atk', atk)
    elseif damage > atk * 0.9 then
        damage = atk * math.random(85,95)/100
        -- cxlog_info('>0.9',damage , 'atk', atk)
    end
    return damage
end


function formula_calc_atk_night_damage(damage, is_night)
    if is_night then
        return damage * 0.8
    else
        return damage
    end
end

function ActorMT:GetAttackDamage(is_critical, is_combo, combo_coef, actor_type_coef)
    local target = self:GetTarget()
    local atk = self:CalcAttack()
    local def = target:CalcDefend()
    local damage = formula_calc_atk_base_damage(atk, def, is_critical, is_combo, combo_coef, actor_type_coef)
    damage = formula_calc_atk_float_damage(atk,damage)
    return damage
end

--大多数法术伤害结果＝武器伤害/4＋灵力差＋法伤系数×技能等级＋初始值
--Damage=WD(weapon damage)/4+ΔF+K*SKILL+I

-- [梦幻西游法伤的计算公式？（包括修炼的计算公式。）]https://zhidao.baidu.com/question/568287506.html?qbl=relate_question_0&word=%C3%CE%BB%C3%CE%F7%D3%CE%C8%CB%CE%EF%B7%A8%C9%CB%B7%A8%B7%C0%D4%F5%C3%B4%BC%C6%CB%E3
--  https://zhidao.baidu.com/question/1302219466168637659.html
--  https://gameinstitute.qq.com/community/detail/100032
-- [在《梦幻西游》里都有哪些经典的数值公式？]https://www.gameres.com/849887.html
-- 

-- 法宠(单法)=等级×3+灵力差×1.2+20
-- 法宠(群法)=(等级×3+灵力差×1.2+10)×（10-作用人数）/10

function ActorMT:GetSpellDamage(target)
    local spell_atk = self:CalcSpiritual()
    local spell_def = target:CalcDefend()
    local lv = self:GetProperty(PROP_LV)
    local damage = lv*3 + (spell_atk-spell_def)*1.2 + 20
    return damage
end


function ActorMT:SetGlobalStandardEquip(lv, is_enforce)
    local equip_tbl = content_system_get_table('equip')
    local x = lv // 10
    local enforce_suffix = is_enforce and '_enforced' or '' 
    
    local weapon = { damage = 0, target = 0 }
    local weapon_tpl = equip_tbl['weapon'..enforce_suffix]
    weapon.target = weapon_tpl.base_target + x * weapon_tpl.lv_accum_target
    weapon.damage = weapon_tpl.base_damage + x * weapon_tpl.lv_accum_damage

    local armor = { defend = 0}
    local armor_tpl = equip_tbl['armor'..enforce_suffix]
    armor.defend = armor_tpl.base_defend + x * armor_tpl.lv_accum_defend
    
    local helmet = {defend = 0, mp = 0}
    local helmet_tpl = equip_tbl['helmet'..enforce_suffix]
    helmet.defend = helmet_tpl.base_defend + x * helmet_tpl.lv_accum_defend
    helmet.mp = helmet_tpl.base_mp + x * helmet_tpl.lv_accum_mp


    local belt = {defend = 0, hp = 0}
    local belt_tpl = equip_tbl['belt'..enforce_suffix]
    belt.defend = belt_tpl.base_defend + x * belt_tpl.lv_accum_defend
    belt.hp = belt_tpl.base_hp + x * belt_tpl.lv_accum_hp

    local necklace = {spiritual = 0}
    local necklace_tpl = equip_tbl['necklace'..enforce_suffix]
    necklace.spiritual = necklace_tpl.base_spiritual + x * necklace_tpl.lv_accum_spiritual

    local shoes = {defend = 0 , aglie = 0}
    local shoes_tpl = equip_tbl['shoes'..enforce_suffix]
    shoes.defend = shoes_tpl.base_defend + x * shoes_tpl.lv_accum_defend
    shoes.aglie = shoes_tpl.base_agile + x * shoes_tpl.lv_accum_agile

    local total_hp = belt.hp
    local total_mp = helmet.mp
    local total_target = weapon.target
    local total_damage = weapon.damage
    local total_defend = armor.defend + helmet.defend + belt.defend+ shoes.defend
    local total_spritual = necklace.spiritual
    local total_aglie = shoes.aglie


    self:SetProperty(PROP_EQUIP_HP, total_hp) 
    self:SetProperty(PROP_EQUIP_MP, total_mp) 
    self:SetProperty(PROP_EQUIP_TARGET, total_target) 
    self:SetProperty(PROP_EQUIP_DAMAGE, total_damage) 
    self:SetProperty(PROP_EQUIP_DEFEND, total_defend) 
    self:SetProperty(PROP_EQUIP_SPIRITUAL, total_spritual) 
    self:SetProperty(PROP_EQUIP_AGILE, total_aglie) 

end
