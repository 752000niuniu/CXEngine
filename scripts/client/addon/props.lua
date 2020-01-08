local ui_is_show_props = false
function ui_toggle_show_props()
    ui_is_show_props = not ui_is_show_props
    return ui_is_show_props
end


local selected_actor_uid = 0
local edit_prop_lv = 0
function ui_show_props()
    if not ui_is_show_props then return end
    imgui.Begin('ActorInfo')
    local actors = actor_manager_fetch_all_actors()
    imgui.HorizontalLayout(actors,next,function(k,v) 
        local selected = imgui.RadioButton(v:GetName()..'##'..v:GetID(), v:GetID() == selected_actor_uid)
        if selected then
            selected_actor_uid = v:GetID()
        end
    end)

    local actor = actor_manager_fetch_player_by_id(selected_actor_uid)
    if actor then
        edit_prop_lv = actor:GetProperty(PROP_LV)
        imgui.PushItemWidth(80)
        res , edit_prop_lv =  imgui.DragInt('等级', edit_prop_lv)
        imgui.PopItemWidth()
        if res then
            net_manager_player_dostring(string.format([[ 
                local actor = actor_manager_fetch_player_by_id(%d)
                actor:SetProperty(PROP_LV, %d)
                actor:SetPropsByPlan({
                    health = 0,
                    magic = 0,
                    force = 5,
                    stamina = 0,
                    agility = 0,
                })

                if actor:GetProperty(PROP_ACTOR_TYPE)==ACTOR_TYPE_SUMMON then
                    actor:SetProperty(PROP_SUMMON_ATK_QUAL ,1400)
                    actor:SetProperty(PROP_SUMMON_DEF_QUAL ,1400)
                    actor:SetProperty(PROP_SUMMON_HEALTH_QUAL ,4500)
                    actor:SetProperty(PROP_SUMMON_MAGIC_QUAL , 2500)
                    actor:SetProperty(PROP_SUMMON_SPEED_QUAL ,1200)
                    actor:SetProperty(PROP_SUMMON_DODGE_QUAL , 1200)
                    actor:SetProperty(PROP_SUMMON_GROW_COEF , 1.25)
                else
                    local lv = actor:GetProperty(PROP_LV)
                    actor:SetProperty(PROP_SCHOOL_SKILL_LV_TARGETHIT, lv) 
                    actor:SetProperty(PROP_SCHOOL_SKILL_LV_DAMAGE, lv) 
                    actor:SetProperty(PROP_SCHOOL_SKILL_LV_DEFEND, lv) 
                    actor:SetProperty(PROP_SCHOOL_SKILL_LV_SPEED, lv) 
                    actor:SetProperty(PROP_SCHOOL_SKILL_LV_DODGE, lv) 
                    actor:SetProperty(PROP_SCHOOL_SKILL_LV_SPIRITUAL, lv) 
                    actor:SetGlobalStandardEquip(lv)
                end
            
                actor:SetProperty(PROP_HP, actor:GetMaxHP())
            ]], actor:GetID() , edit_prop_lv))
        end

        imgui.SameLine()

        if actor:GetProperty(PROP_ACTOR_TYPE) ==  ACTOR_TYPE_PLAYER then
            imgui.Text('种族:'..actor:GetRaceName())
            imgui.SameLine()
            imgui.Text('门派:'..actor:GetSchoolName())    
        end

        imgui.Text('HP:'.. math.floor(actor:GetProperty(PROP_HP)) .. '/'..actor:GetMaxHP())
        imgui.SameLine()
        imgui.Text('MP:' .. math.floor(actor:GetProperty(PROP_MP)) ..'/' ..actor:GetMaxMP())
        if actor:GetProperty(PROP_ACTOR_TYPE) ==  ACTOR_TYPE_PLAYER then
            imgui.SameLine()
            imgui.Text('SP:'..actor:GetProperty(PROP_SP) ..'/150')
        end
        imgui.Separator()

        imgui.BeginGroup()
        imgui.Dummy(30,20)
        imgui.Text('体质 '..actor:GetProperty(PROP_BASE_HEALTH))
        imgui.Text('魔力 '..actor:GetProperty(PROP_BASE_MAGIC))
        imgui.Text('力量 '..actor:GetProperty(PROP_BASE_FORCE))
        imgui.Text('耐力 '..actor:GetProperty(PROP_BASE_STAMINA))
        imgui.Text('敏捷 '..actor:GetProperty(PROP_BASE_AGILITY))
        imgui.EndGroup()

        imgui.SameLine()
        imgui.Dummy(30,10)
        imgui.SameLine()

        imgui.BeginGroup()
        imgui.Dummy(30,10)
        if actor:GetProperty(PROP_ACTOR_TYPE)  == ACTOR_TYPE_PLAYER then
            imgui.Text(string.format('命中 %.1f', actor:CalcTargetHit()) )
        end
        imgui.Text(string.format('攻击 %.1f', actor:CalcAttack()) )
        imgui.Text(string.format('防御 %.1f', actor:CalcDefend()) )
        imgui.Text(string.format('灵力 %.1f', actor:CalcSpiritual()) )
        imgui.Text(string.format('速度 %.1f', actor:CalcSpeed()) )
        imgui.Text(string.format('躲闪 %.1f', actor:CalcDodge()) )
        imgui.EndGroup()

        imgui.SameLine()
        imgui.Dummy(30,10)
        imgui.SameLine()
        if actor:GetProperty(PROP_ACTOR_TYPE)  == ACTOR_TYPE_PLAYER then
            imgui.BeginGroup()
            imgui.Text('HP技能等级:'..actor:GetProperty(PROP_SCHOOL_SKILL_LV_HP))
            imgui.Text('MP技能等级:'..actor:GetProperty(PROP_SCHOOL_SKILL_LV_MP))
            imgui.Text('命中技能等级:'..actor:GetProperty(PROP_SCHOOL_SKILL_LV_TARGETHIT))
            imgui.Text('伤害技能等级:'..actor:GetProperty(PROP_SCHOOL_SKILL_LV_DAMAGE))
            imgui.Text('防御技能等级:'..actor:GetProperty(PROP_SCHOOL_SKILL_LV_DEFEND))
            imgui.Text('灵力技能等级:'..actor:GetProperty(PROP_SCHOOL_SKILL_LV_SPIRITUAL))
            imgui.Text('速度技能等级:'..actor:GetProperty(PROP_SCHOOL_SKILL_LV_SPEED))
            imgui.Text('躲避技能等级:'..actor:GetProperty(PROP_SCHOOL_SKILL_LV_DODGE))
            imgui.EndGroup()
            imgui.Separator()
        end
        if actor:GetProperty(PROP_ACTOR_TYPE) == ACTOR_TYPE_SUMMON then
            imgui.BeginGroup()
            imgui.Text('攻击资质 '..actor:GetProperty(PROP_SUMMON_ATK_QUAL))
            imgui.Text('防御资质 '..actor:GetProperty(PROP_SUMMON_DEF_QUAL))
            imgui.Text('体力资质 '..actor:GetProperty(PROP_SUMMON_HEALTH_QUAL))
            imgui.Text('法力资质 '..actor:GetProperty(PROP_SUMMON_MAGIC_QUAL))
            imgui.Text('速度资质 '..actor:GetProperty(PROP_SUMMON_SPEED_QUAL))
            imgui.Text('躲闪资质 '..actor:GetProperty(PROP_SUMMON_DODGE_QUAL))
            imgui.Text('成长 '    ..actor:GetProperty(PROP_SUMMON_GROW_COEF))
            imgui.EndGroup()
        end
    end
    imgui.End()
end