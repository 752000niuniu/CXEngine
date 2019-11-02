
function SkillOnStart(skill, actor, target)

end

function SkillOnHit(skill, actor, target)
    target:AddBuffer(1,skill.turn)
end

function SkillOnEnd(skill, actor, target)

end