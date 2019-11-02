
function SkillOnStart(skill, actor, target)

end

function SkillOnHit(skill, actor, hit_actor)
    hit_actor:AddBuffer(1,skill.turn)
end

function SkillOnEnd(skill, actor, target)

end