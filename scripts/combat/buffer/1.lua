function BufferOnStart(buffer, actor, turn)
    local anim = animation_create(23,0xCA8FDEAD)
    buffer.anim = anim 
    anim:SetLoop(0)
-- local offy = -avatar:GetFrameKeyY() + avatar:GetFrameHeight() / 2.0
    anim:SetOffsetY(10)  
    actor:AddBackAnim(anim)
end

function BufferOnEnd(buffer, actor, target)

end

function BufferOnNextTurn(buffer, actor, turn)
    cxlog_info('BufferOnNextTurn', turn , buffer.add_turn)
    if turn - buffer.add_turn == 3 then
        buffer.anim:Stop()
    end
end