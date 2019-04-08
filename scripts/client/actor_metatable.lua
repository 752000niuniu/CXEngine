local ActorMT = actor_get_metatable()

local pos  = {1,3}
function ActorMT:GetNickName()
    return self:GetProperty('name')
end

function ActorMT:SetPosition(x, y)
    pos[1] = x
    pos[2] = y
end


function ActorMT:GetPosition()
    return pos[1], pos[2]
end

