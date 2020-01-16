
local BufferMT = {}
function BufferMT:new(o)
    o = o or {}
    self.__index = self 
    setmetatable(o, self)
    return o
end

local buffer_env = {
    __index = _ENV
}

local buffer_table = {}
function init_buffers()
    local buff_template_tbl = content_system_get_table('buffer')
    buffer_table = buff_template_tbl
    for id,buff in pairs(buffer_table) do
        local dir = vfs_get_luapath('../combat/buffer/')
        local path = dir..id..'.lua'
        local f = io.open(path)
        if f then
            local chunk = f:read('a')
            f:close()
            local module = setmetatable({},buffer_env)
            local fun,err = load(chunk,'@'..path,'bt', module)
            if fun then
                fun()
                buff.BufferOnStart = module.BufferOnStart
                buff.BufferOnEnd = module.BufferOnEnd
                buff.BufferOnNextTurn = module.BufferOnNextTurn
            else
                cxlog_info(fun,err)
            end
        end 
    end
end


local ActorMT = actor_get_metatable()
local Buffers = {}
function ActorMT:GetBuffers()
    local id = self:GetID()
    if not Buffers[id] then 
        Buffers[id] = {}
    end
    return Buffers[id]
end

function ActorMT:AddBuffer(buff_id, turn_cnt)
    local buff_tpl = buffer_table[buff_id]
    if not buff_tpl then return end
    local buff = {}
    buff.tid = buff_id
    buff.turn_cnt = turn_cnt
    buff.turn_counter = 0
    local buffers = self:GetBuffers()
    buffers[buff_id] = buff
    if IsClient() then
        local templ = buffer_table[buff_id]
        if templ.BufferOnStart then
            templ.BufferOnStart(buff,self)
        end
    end
end

function ActorMT:RemoveBuffer(buff_id)
    local buffers = self:GetBuffers()
    local buff = buffers[buff_id]
    if not buff then return end
    if IsClient() then
        local templ = buffer_table[buff_id]
        if templ.BufferOnEnd then
            templ.BufferOnEnd(buff,self)
        end
    end

    buffers[buff_id] = nil
end

function ActorMT:BufferNextTurn()
    local buffers = self:GetBuffers()
    if buffers then
        for id,buff in pairs(buffers) do
            buff.turn_counter = buff.turn_counter + 1
            if IsClient() then
                local templ = buffer_table[id]
                if templ.BufferOnNextTurn then
                    templ.BufferOnNextTurn(buff,self)
                end     
            end
            if buff.turn_counter == buff.turn_cnt then
                self:RemoveBuffer(id)
            end
        end
    end
end