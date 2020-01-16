function BufferOnStart(buff, actor, target)
    if IsClient() then
        local anim = animation_create(23,0xD9463A0C)
        anim:SetLoop(0)
        -- local offy = -avatar:GetFrameKeyY() + avatar:GetFrameHeight() / 2.0
        anim:SetOffsetY(10)
        buff.anim = anim 
        buff.turn_cnt = 2
        actor:AddBackAnim(anim)
    end
end

function BufferOnEnd(buff, actor, target)
    if IsClient() then
        buff.anim:Stop()
    end
end
