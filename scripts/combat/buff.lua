
local BuffMT = {}
function BuffMT:new(o)
    o = o or {}
    self.__index = self 
    setmetatable(o, self)
    return o
end

local buff_env = {
    __index = _ENV
}

local buff_table = {}
function init_buffs()
    local buff_template_tbl = content_system_get_table('buff')
    buff_table = buff_template_tbl
    for id,buff in pairs(buff_table) do
        local dir = vfs_get_luapath('../combat/buff/')
        local path = dir..id..'.lua'
        local f = io.open(path)
        if f then
            local chunk = f:read('a')
            f:close()
            local module = setmetatable({},buff_env)
            local fun,err = load(chunk,'@'..path,'bt', module)
            if fun then
                fun()
                buff.BuffOnStart = module.BuffOnStart
                buff.BuffOnEnd = module.BuffOnEnd
                buff.BuffOnNextTurn = module.BuffOnNextTurn
            else
                cxlog_info(fun,err)
            end
        end 
    end
end

local ActorMT = actor_get_metatable()
local Buffs = {}
function ActorMT:GetBuffs()
    local id = self:GetID()
    if not Buffs[id] then 
        Buffs[id] = {}
    end
    return Buffs[id]
end

function ActorMT:AddBuff(buff_id, turn_cnt)
    local buff_tpl = buff_table[buff_id]
    if not buff_tpl then return end
    local buff = {}
    buff.tid = buff_id
    buff.turn_cnt = turn_cnt
    buff.turn_counter = 0
    local buffs = self:GetBuffs()
    buffs[buff_id] = buff
    local templ = buff_table[buff_id]
    if templ.BuffOnStart then
        templ.BuffOnStart(buff,self)
    end
end

function ActorMT:RemoveBuff(buff_id)
    local buffs = self:GetBuffs()
    local buff = buffs[buff_id]
    if not buff then return end
    local templ = buff_table[buff_id]
    if templ.BuffOnEnd then
        templ.BuffOnEnd(buff,self)
    end

    buffs[buff_id] = nil
end

function ActorMT:BuffNextTurn()
    local buffs = self:GetBuffs()
    if buffs then
        for id,buff in pairs(buffs) do
            buff.turn_counter = buff.turn_counter + 1
            local templ = buff_table[id]
            if templ.BuffOnNextTurn then
                templ.BuffOnNextTurn(buff,self)
            end     
            if buff.turn_counter == buff.turn_cnt then
                self:RemoveBuff(id)
            end
        end
    end
end