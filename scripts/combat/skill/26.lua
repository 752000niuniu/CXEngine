
function SkillOnStart(skill, actor, target)

end

function SkillOnHit(skill, actor, hit_actor)
    -- hit_actor:AddBuff(1,skill.turn)
    if IsServer() then
        actor_add_buff(hit_actor, 30)
    end
    if IsClient() then
        actor_add_buff(hit_actor, 30)
    end
end

function SkillOnEnd(skill, actor, target)

end