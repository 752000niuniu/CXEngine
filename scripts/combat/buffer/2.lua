function BufferOnStart(buffer, actor, target)
    local anim = animation_create(23,0xD9463A0C)
    anim:SetLoop(0)
    -- local offy = -avatar:GetFrameKeyY() + avatar:GetFrameHeight() / 2.0
    anim:SetOffsetY(10)
    buffer.anim = anim 
    actor:AddBackAnim(anim)
end

function BufferOnEnd(buffer, actor, target)
    
end

function BufferOnNextTurn(buffer, actor, turn)
    if turn - buffer.add_turn == 1 then
        buffer.anim:Stop()
    end
end