
function SkillOnStart(skill, actor, target)

end

function SkillOnHit(skill, master, target, target_i, combo_i)
    if IsClient() then
        local atk_info = skill.effects[target_i]
        if combo_i == atk_info.combo then
            master:AddBuff(2,2)
        end
    end
end

function SkillOnEnd(skill, actor, target)

end