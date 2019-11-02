function BufferOnStart(buffer, actor, target)
    local anim = animation_create(23,0xCA8FDEAD)
    anim:SetLoop(0)
-- local offy = -avatar:GetFrameKeyY() + avatar:GetFrameHeight() / 2.0
    anim:SetOffsetY(10)  
    actor:AddBackAnim(anim)
end

function BufferOnEnd(buffer, actor, target)

end