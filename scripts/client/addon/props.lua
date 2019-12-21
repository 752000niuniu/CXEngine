local ui_is_show_props = false
function ui_toggle_show_props()
    ui_is_show_props = not ui_is_show_props
    return ui_is_show_props
end

function ui_show_props()
    if not ui_is_show_props then return end
    imgui.Begin('ActorInfo')
    if imgui.CollapsingHeader('ActorInfo') then
        local actor = actor_manager_fetch_local_player()
        if actor then
            local actor_type = actor:GetProperty(PROP_ACTOR_TYPE) 
            for i = 1, 5 do 
                local selected = imgui.RadioButton(i..'##rbplan',cur_add_plan == i) 
                if i ~= 5 then
                    imgui.SameLine()
                end
                if selected then
                    cur_add_plan = i
                    actor:SetProperty(PROP_ADD_PROP_PLAN, add_plan_map[cur_add_plan])
                    cxlog_info('cur_add_plan',cur_add_plan,add_plan_map[cur_add_plan])
                end
            end

            imgui.Text('等级 '..actor:GetProperty(PROP_LV))
            imgui.SameLine()
            if imgui.Button('+5##lv') then
                local lv = actor:GetProperty(PROP_LV) + 5
                actor:SetProperty(PROP_LV, lv)
                if actor==ACTOR_TYPE_PLAYER then
                    actor:UpdatePropPtsByPlan()
                end
            end
            imgui.SameLine()
            if imgui.Button('-5##lv') then
                local lv = actor:GetProperty(PROP_LV) - 5
                actor:SetProperty(PROP_LV, lv)
                if actor==ACTOR_TYPE_PLAYER then
                    actor:UpdatePropPtsByPlan()
                end
            end

            if imgui.Button('固定计算') then
                if actor_type==ACTOR_TYPE_SUMMON then
                    actor:SetProperty(PROP_SUMMON_ATK_QUAL ,1400)
                    actor:SetProperty(PROP_SUMMON_DEF_QUAL ,1400)
                    actor:SetProperty(PROP_SUMMON_HEALTH_QUAL ,4500)
                    actor:SetProperty(PROP_SUMMON_MAGIC_QUAL , 2500)
                    actor:SetProperty(PROP_SUMMON_SPEED_QUAL ,1200)
                    actor:SetProperty(PROP_SUMMON_DODGE_QUAL , 1200)
                    actor:SetProperty(PROP_SUMMON_GROW_COEF , 1.25)

                    actor:SetProperty(PROP_BASE_HEALTH, 417 )
                    actor:SetProperty(PROP_BASE_MAGIC,680 )
                    actor:SetProperty(PROP_BASE_FORCE, 171)
                    actor:SetProperty(PROP_BASE_STAMINA , 171 )
                    actor:SetProperty(PROP_BASE_AGILITY, 171)
                    actor:SetProperty(PROP_LV, 151)
                else
                    local lv = 69
                    actor:SetPropsByPlan({
                        health = 0,
                        magic = 0,
                        force = 5,
                        stamina = 0,
                        agility = 0,
                    })

                    actor:SetProperty(PROP_LV, lv)
                    actor:SetProperty(PROP_SCHOOL_SKILL_LV_TARGETHIT, lv) 
                    actor:SetProperty(PROP_SCHOOL_SKILL_LV_DAMAGE, lv) 
                    actor:SetProperty(PROP_SCHOOL_SKILL_LV_DEFEND, lv) 
                    actor:SetProperty(PROP_SCHOOL_SKILL_LV_SPEED, lv) 
                    actor:SetProperty(PROP_SCHOOL_SKILL_LV_DODGE, lv) 
                    actor:SetProperty(PROP_SCHOOL_SKILL_LV_SPIRITUAL, 60) 

                    actor:SetGlobalStandardEquip(lv)
                end
            end
            imgui.Text('种族:'..actor:GetRaceName())
            imgui.SameLine()
            imgui.Text('门派:'..actor:GetSchoolName())
            
            imgui.Text('HP:'.. math.floor(actor:GetProperty(PROP_HP)) .. '/'..actor:GetMaxHP())
            imgui.SameLine()
            imgui.Text('MP:' .. math.floor(actor:GetProperty(PROP_MP)) ..'/' ..actor:GetMaxMP())
            imgui.SameLine()
            imgui.Text('SP:'..actor:GetProperty(PROP_SP) ..'/150')
            imgui.Separator()

            imgui.Text('体质 '..actor:GetProperty(PROP_BASE_HEALTH))
            imgui.Text('魔力 '..actor:GetProperty(PROP_BASE_MAGIC))
            imgui.Text('力量 '..actor:GetProperty(PROP_BASE_FORCE))
            imgui.Text('耐力 '..actor:GetProperty(PROP_BASE_STAMINA))
            imgui.Text('敏捷 '..actor:GetProperty(PROP_BASE_AGILITY))
            imgui.Separator()
            
            imgui.Text('命中技能等级:'..actor:GetProperty())
            
            imgui.Text('HP技能等级:'..actor:GetProperty(PROP_SCHOOL_SKILL_LV_HP))
            imgui.Text('MP技能等级:'..actor:GetProperty(PROP_SCHOOL_SKILL_LV_MP))
            imgui.Text('命中技能等级:'..actor:GetProperty(PROP_SCHOOL_SKILL_LV_TARGETHIT))
            imgui.Text('伤害技能等级:'..actor:GetProperty(PROP_SCHOOL_SKILL_LV_DAMAGE))
            imgui.Text('防御技能等级:'..actor:GetProperty(PROP_SCHOOL_SKILL_LV_DEFEND))
            imgui.Text('灵力技能等级:'..actor:GetProperty(PROP_SCHOOL_SKILL_LV_SPIRITUAL))
            imgui.Text('速度技能等级:'..actor:GetProperty(PROP_SCHOOL_SKILL_LV_SPEED))
            imgui.Text('躲避技能等级:'..actor:GetProperty(PROP_SCHOOL_SKILL_LV_DODGE))

            imgui.Separator()
            imgui.Text('命中 '..actor:CalcTargetHit())
            imgui.Text('攻击 '..actor:CalcAttack())
            imgui.Text('防御 '..actor:CalcDefend())
            imgui.Text('灵力 '..actor:CalcSpiritual())
            imgui.Text('速度 '..actor:CalcSpeed())
            imgui.Text('躲闪 '..actor:CalcDodge())

            if actor:GetProperty(PROP_ACTOR_TYPE) == ACTOR_TYPE_SUMMON then
                imgui.Text('攻击资质 '..actor:GetProperty(PROP_SUMMON_ATK_QUAL))
                imgui.Text('防御资质 '..actor:GetProperty(PROP_SUMMON_DEF_QUAL))
                imgui.Text('体力资质 '..actor:GetProperty(PROP_SUMMON_HEALTH_QUAL))
                imgui.Text('法力资质 '..actor:GetProperty(PROP_SUMMON_MAGIC_QUAL))
                imgui.Text('速度资质 '..actor:GetProperty(PROP_SUMMON_SPEED_QUAL))
                imgui.Text('躲闪资质 '..actor:GetProperty(PROP_SUMMON_DODGE_QUAL))
                imgui.Text('成长 '    ..actor:GetProperty(PROP_SUMMON_GROW_COEF))
            end
        end
    end
    imgui.End()
end