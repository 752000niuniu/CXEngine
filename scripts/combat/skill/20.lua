
function SkillOnStart(skill, actor, target)

end

function SkillOnHit(skill, actor, hit_actor)
    if skill.atk_counter == skill.atk_cnt then
        actor:AddBuffer(2,2)
    end
end

function SkillOnEnd(skill, actor, target)

end