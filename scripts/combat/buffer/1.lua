function BufferOnStart(buffer, actor, turn)
    local anim = animation_create(23,0xCA8FDEAD)
    buffer.anim = anim 
    anim:SetLoop(0)
-- local offy = -avatar:GetFrameKeyY() + avatar:GetFrameHeight() / 2.0
    actor:AddBackAnim(anim)
end

function BufferOnEnd(buffer, actor, target)
    buffer.anim:Stop()
end
