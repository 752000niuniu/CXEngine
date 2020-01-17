
function SkillOnStart(skill, actor, target)
    -- 
end

function SkillOnHit(skill, actor, hit_actor)
    hit_actor:AddBuff(1,skill.turn)
end

function SkillOnSpell(skill, actor, hit_actor)
    skill.spell_anim:SetOffsetY(-30)  
end

function SkillOnAfterSpell(skill, actor, hit_actor)
    hit_actor:AddBuff(1,2)
end

function SkillOnEnd(skill, actor, target)

end