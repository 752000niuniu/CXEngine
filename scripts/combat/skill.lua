
local SkillMT = {}
function SkillMT:new(o)
    o = o or {}
    self.__index = self 
    setmetatable(o, self)
    return o
end

local skill_env = {
    __index = _ENV
}

local skill_table = {}
function init_skills()
    local dir = vfs_get_luapath('../combat/skill')
    local files = vfs_list_files(dir)
    for i,path in ipairs(files) do
        -- cxlog_info(i,id, type(id), path)
        local id = math.tointeger(path:match('/(%d+)%.lua')) 
        local module = setmetatable({},skill_env)
        local fun,err = loadfile(path,'bt', module)
        if fun then
            fun()
        else
            cxlog_info(fun,err)
        end
        skill_table[id] = module
    end
end

function skill_on_start(skill, actor, target)
    if skill_table[skill.ID] then
        if skill_table[skill.ID].SkillOnStart then
            skill_table[skill.ID].SkillOnStart(skill, actor, target)
        end
    end
end

function skill_on_end(skill, actor, target)
    if skill_table[skill.ID] then
        if skill_table[skill.ID].SkillOnEnd then
            skill_table[skill.ID].SkillOnEnd(skill, actor, target)
        end
    end
end

function skill_on_hit(skill, actor, target)
    if skill_table[skill.ID] then
        if skill_table[skill.ID].SkillOnHit then
            skill_table[skill.ID].SkillOnHit(skill, actor, target)
        end
    end
end