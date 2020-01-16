function BufferOnStart(buffer, actor, turn)
    if IsClient() then
        local anim = animation_create(23,0xCA8FDEAD)
        buffer.anim = anim 
        anim:SetLoop(0)
    -- local offy = -avatar:GetFrameKeyY() + avatar:GetFrameHeight() / 2.0
        actor:AddBackAnim(anim)
    end
end

function BufferOnEnd(buffer, actor, target)
    if IsClient() then
        buffer.anim:Stop()
    end
end
