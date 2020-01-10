
local SkillMT = {}
local skill_table = {}
function SkillMT:new(o)
    o = o or {
        id = utils_next_uid('skill')
    }
    self.__index = self 
    setmetatable(o, self)
    return o
end

function SkillMT:GetTemplate()
    return skill_table[self.tid] 
end

function SkillMT:GetType()
    return skill_table[self.tid].type
end

local skill_env = {
    __index = _ENV
}

function init_skills()
    local skill_template_tbl = content_system_get_table('skill')
    skill_table = skill_template_tbl
    for id,skill in pairs(skill_table) do
        local dir = vfs_get_luapath('../combat/skill/')
        local path = dir..id..'.lua'
        local f = io.open(path)
        if f then
            local chunk = f:read('a')
            f:close()
            local module = setmetatable({},skill_env)
            local fun,err = load(chunk,'@'..path,'bt', module)
            if fun then
                fun()
                skill.SkillOnStart = module.SkillOnStart
                skill.SkillOnEnd = module.SkillOnEnd
                skill.SkillOnHit = module.SkillOnHit
                skill.SkillOnSpell = module.SkillOnSpell
                skill.SkillOnAfterSpell = module.SkillOnAfterSpell
            else
                cxlog_info(fun,err)
            end
        end 
    end
end

__skills__ = __skills__ or {}

local ActorMT = actor_get_metatable()
function ActorMT:CastSkill(skill_id)
    local actor = self
    local skill_tpl = skill_table[skill_id]
    if not skill_tpl then return end
    local battle = self:GetBattle()
    if not battle then return end
    cxlog_info('cast skill ',skill_id)
    local skill = SkillMT:new()
    __skills__[skill.id] = skill
    self:SetUsingSkill(skill.id)

    skill.caster_end = false
    skill.target_end = false
    skill.tid = skill_id
    skill.turn = battle.turn
    skill.templ = skill_tpl
    skill.group_kill = skill.templ.group_kill
    skill.combo = skill.templ.combo
    skill.type = skill.templ.type
    
    if skill.type == 'atk' then
        if skill.group_kill > 0 then
			if IsClient() then
                on_cast_group_attack(skill, actor)
            end
        else
            skill.atk_cnt = 0
            skill.atk_damage = {}
            if skill.combo > 0 then
                skill.atk_cnt = skill.combo
            else
                skill.atk_cnt = 1
            end
            local target = actor:GetTarget()
            for i=1, skill.atk_cnt do
                local damage = actor:GetAttackDamage(false,false,0,1)
                table.insert(skill.atk_damage, damage)
                target:ModifyHP(-damage)
                if target:IsDead() then
                    skill.atk_cnt = i
                    break
                end
            end
			if IsClient() then
                on_cast_attack(skill, actor)
            end
        end
    elseif skill.type == 'spell' then
        local to_self_group = false
        if skill.sub_type == SKILL_SUBTYPE_DEFAULT or skill.sub_type == SKILL_SUBTYPE_SEAL then
            to_self_group = false
        elseif skill.sub_type == SKILL_SUBTYPE_HEAL or skill.sub_type == SKILL_SUBTYPE_AUXI then
            to_self_group = true
        end
        local targets = {}
        for i, actor in ipairs(battle.actors) do
            if to_self_group then
                if actor:GetProperty(PROP_TEAM_TYPE) == self:GetProperty(PROP_TEAM_TYPE) then
                    table.insert(targets,actor)
                end    
            else
                if actor:GetProperty(PROP_TEAM_TYPE) ~= self:GetProperty(PROP_TEAM_TYPE) then
                    table.insert(targets,actor)
                end    
            end
        end
        table.sort(targets,function(a,b)
            return a:CalcSpeed() > b:CalcSpeed()
        end)
        
        if skill.templ.group_kill > 0 then
            skill.spell_cnt = math.min(#targets,skill.group_kill) 
            skill.spell_actors = { }
            for i=1,#targets do
                local target = targets[i]
                if target:GetID() ~= self:GetTarget():GetID() and #skill.spell_actors < skill.spell_cnt-1 then
                    table.insert(skill.spell_actors, target)
                end
            end
            table.insert(skill.spell_actors,self:GetTarget())
        else
            skill.spell_cnt = 1
            skill.spell_actors = {}
            table.insert(skill.spell_actors,self:GetTarget())
        end
        
        skill.spell_damage = {}
        for i=1,skill.spell_cnt do
            local target = skill.spell_actors[i]
            local damage = actor:GetSpellDamage(target)
            if skill.templ.sub_type == SKILL_SUBTYPE_SEAL then
                damage = 0            
            elseif  skill.templ.sub_type == SKILL_SUBTYPE_HEAL then
                damage = -damage
            end
            table.insert(skill.spell_damage,damage)
            target:ModifyHP(-damage)
        end

		skill.atk_anim = skill.templ.atk_anim
		if IsClient() then
			on_cast_spell(skill,actor)
		end
    end
end

function ActorMT:SetUsingSkill(skill_uid)
    self:SetProperty(PROP_USING_SKILL, skill_uid)
end

function ActorMT:GetUsingSkill()
    local id = self:GetProperty(PROP_USING_SKILL)
    return __skills__[id]
end

function ActorMT:EndUsingSkill()
    local id = self:GetProperty(PROP_USING_SKILL)
    __skills__[id] = nil
    self:SetProperty(PROP_USING_SKILL, 0)
end

function ActorMT:IsUsingSkill()
    return self:GetProperty(PROP_IS_USING_SKILL)
end
