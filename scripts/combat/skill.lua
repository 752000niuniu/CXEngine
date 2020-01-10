
SkillMT = {}
skill_table = {}
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
