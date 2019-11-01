
function SkillOnStart(skill, actor, target)

end

function SkillOnHit(skill, actor, target)
    local anim = animation_create(23,0xCA8FDEAD)
    anim:SetLoop(1)
-- local offy = -avatar:GetFrameKeyY() + avatar:GetFrameHeight() / 2.0
    anim:SetOffsetY(10)  
    target:AddStateAnim(anim)
end

function SkillOnEnd(skill, actor, target)

end