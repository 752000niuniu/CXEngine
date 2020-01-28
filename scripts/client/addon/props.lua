local ui_is_show_props = false
function ui_toggle_show_props()
    ui_is_show_props = not ui_is_show_props
    return ui_is_show_props
end


local selected_actor_uid = 0
local edit_prop_lv = 0

local edit_health = 0
local edit_magic = 0
local edit_force = 0
local edit_stamina = 0
local edit_agility = 0

function draw_prop_bar(actor, prop, total, edit_prop)
    local remain = total - edit_health - edit_magic - edit_force - edit_stamina - edit_agility
    imgui.Text(prop[1].. ': '.. prop[3])
    imgui.SameLine()
    imgui.PushItemWidth(80)
    local res
    if remain+edit_prop == 0 then
        res, edit_prop = imgui.DragInt('##bar'..prop[1], edit_prop, 1, -1, 0)
    else
        res, edit_prop = imgui.DragInt('##bar'..prop[1], edit_prop, 1, 0, remain + edit_prop)
    end
    imgui.PopItemWidth()
    return edit_prop
end

function draw_prop_points_panel(actor, lv)
    local res 
    local total = (lv+1) * 5

    edit_health = actor:GetProperty(PROP_ASSIGN_HEALTH)
    edit_magic = actor:GetProperty(PROP_ASSIGN_MAGIC)
    edit_force = actor:GetProperty(PROP_ASSIGN_FORCE)
    edit_stamina = actor:GetProperty(PROP_ASSIGN_STAMINA)
    edit_agility = actor:GetProperty(PROP_ASSIGN_AGILITY)

    
    imgui.BeginGroup()
    imgui.Dummy(30,20)
    
    local prop_bars = {
        {'体质', PROP_ASSIGN_HEALTH, actor:GetHealthProp()},
        {'魔力', PROP_ASSIGN_MAGIC, actor:GetMagicProp()},
        {'力量', PROP_ASSIGN_FORCE, actor:GetForceProp()},
        {'耐力', PROP_ASSIGN_STAMINA, actor:GetStaminaProp()},
        {'敏捷', PROP_ASSIGN_AGILITY, actor:GetAgilityProp()},
    }
    
    edit_health =  draw_prop_bar(actor, prop_bars[1], total, edit_health)
    edit_magic =   draw_prop_bar(actor, prop_bars[2], total, edit_magic)
    edit_force =   draw_prop_bar(actor, prop_bars[3], total, edit_force)
    edit_stamina = draw_prop_bar(actor, prop_bars[4], total, edit_stamina)
    edit_agility = draw_prop_bar(actor, prop_bars[5], total, edit_agility)

    actor:SetProperty(PROP_ASSIGN_HEALTH, edit_health)
    actor:SetProperty(PROP_ASSIGN_MAGIC, edit_magic)
    actor:SetProperty(PROP_ASSIGN_FORCE, edit_force)
    actor:SetProperty(PROP_ASSIGN_STAMINA, edit_stamina)
    actor:SetProperty(PROP_ASSIGN_AGILITY, edit_agility)

    imgui.EndGroup()

    imgui.Text('剩余点：'.. actor:GetRemainPropPoints())
    imgui.SameLine()
    if imgui.Button('重新加点') then
        net_manager_player_dostring(string.format([[ 
            local actor = actor_manager_fetch_player_by_id(%d)
            actor:ClearAssignPoints()
        ]], actor:GetID()))
    end
        
    imgui.SameLine()
    if imgui.Button('确认加点') then
        net_manager_player_dostring(string.format([[ 
            local actor = actor_manager_fetch_player_by_id(%d)
            actor:SetProperty(PROP_ASSIGN_HEALTH, %d)
            actor:SetProperty(PROP_ASSIGN_MAGIC, %d)
            actor:SetProperty(PROP_ASSIGN_FORCE, %d)
            actor:SetProperty(PROP_ASSIGN_STAMINA, %d)
            actor:SetProperty(PROP_ASSIGN_AGILITY, %d)
        ]], actor:GetID(), edit_health, edit_magic, edit_force, edit_stamina, edit_agility))
    end
end

local prop_school_skill_lv_hp = 0
local prop_school_skill_lv_mp = 0
local prop_school_skill_lv_targethit = 0
local prop_school_skill_lv_damage = 0
local prop_school_skill_lv_defend = 0
local prop_school_skill_lv_spiritual = 0
local prop_school_skill_lv_speed = 0
local prop_school_skill_lv_dodge = 0


function draw_player_skill_bar(actor, bar)
    local actor_lv = actor:GetProperty(PROP_LV)
    imgui.PushItemWidth(50)
    local res, lv = imgui.DragInt(bar[1].. '技能等级',  actor:GetProperty(bar[2]) , 1.0, 0, actor_lv+10)
    imgui.PopItemWidth()
    if res then
        actor:SetProperty(bar[2], lv)
        net_manager_player_dostring(string.format([[ 
            local actor = actor_manager_fetch_player_by_id(%d)
            actor:SetProperty(%d, %d)
        ]], actor:GetID(), bar[2], lv))
    end
end

local pannel = {
    {'HP', PROP_SCHOOL_SKILL_LV_HP },
    {'MP', PROP_SCHOOL_SKILL_LV_MP },
    {'命中', PROP_SCHOOL_SKILL_LV_TARGETHIT },
    {'伤害', PROP_SCHOOL_SKILL_LV_DAMAGE },
    {'防御', PROP_SCHOOL_SKILL_LV_DEFEND },
    {'灵力', PROP_SCHOOL_SKILL_LV_SPIRITUAL },
    {'速度', PROP_SCHOOL_SKILL_LV_SPEED },
    {'闪避', PROP_SCHOOL_SKILL_LV_DODGE },
}

function draw_player_skill_pannel(actor)
    imgui.BeginGroup()
    draw_player_skill_bar(actor, pannel[1])
    draw_player_skill_bar(actor, pannel[2])
    draw_player_skill_bar(actor, pannel[3])
    draw_player_skill_bar(actor, pannel[4])
    draw_player_skill_bar(actor, pannel[5])
    draw_player_skill_bar(actor, pannel[6])
    draw_player_skill_bar(actor, pannel[7])
    draw_player_skill_bar(actor, pannel[8])
    imgui.EndGroup()
end

function draw_player_equip_pannel(actor)
    -- prop_equip_hp	float	0	1
    -- prop_equip_mp	float	0	1
    -- prop_equip_target	float	0	1
    -- prop_equip_damage	float	0	1
    -- prop_equip_defend	float	0	1
    -- prop_equip_spiritual	float	0	1
    -- prop_equip_agile	float	0	1
    imgui.Text('头盔')
    imgui.Text('项链')
    imgui.Text('武器')
    imgui.Text('衣服')
    imgui.Text('腰带')
    imgui.Text('鞋子')
end

function draw_player_practice_lv(actor)
    function draw_practice_lv_bar(label, enum)
        local actor_lv = actor:GetProperty(PROP_LV)
        imgui.PushItemWidth(50)
        local res, lv = imgui.DragInt(label,  actor:GetProperty(enum) , 1.0, 0, math.min(25,actor_lv//5-4))
        imgui.PopItemWidth()
        if res then
            actor:SetProperty(enum, lv)
            net_manager_player_dostring(string.format([[ 
                local actor = actor_manager_fetch_player_by_id(%d)
                actor:SetProperty(%d, %d)
            ]], actor:GetID(), enum, lv))
        end
    end

    draw_practice_lv_bar('攻击修炼等级' , PROP_ATK_PRACTICE_SKILL_LV) 
    draw_practice_lv_bar('防御修炼等级' , PROP_ATK_RESISTANCE_SKILL_LV) 
    draw_practice_lv_bar('法术修炼等级' , PROP_SPELL_PRACTICE_SKILL_LV) 
    draw_practice_lv_bar('法抗修炼等级' , PROP_SPELL_RESISTANCE_SKILL_LV) 
end


function ui_show_props()
    if not ui_is_show_props then return end
    imgui.Begin('ActorInfo')
    local actors = actor_manager_fetch_all_actors()
    imgui.HorizontalLayout(actors,next,function(k,v) 
        local selected = imgui.RadioButton(v:GetName()..'##'..v:GetID(), v:GetID() == selected_actor_uid)
        if selected then
            selected_actor_uid = v:GetID()
            local actor = actor_manager_fetch_player_by_id(selected_actor_uid)
            prop_school_skill_lv_hp = actor:GetProperty(PROP_SCHOOL_SKILL_LV_HP) 
            prop_school_skill_lv_mp = actor:GetProperty(PROP_SCHOOL_SKILL_LV_MP) 
            prop_school_skill_lv_targethit = actor:GetProperty(PROP_SCHOOL_SKILL_LV_TARGETHIT) 
            prop_school_skill_lv_damage = actor:GetProperty(PROP_SCHOOL_SKILL_LV_DAMAGE) 
            prop_school_skill_lv_defend = actor:GetProperty(PROP_SCHOOL_SKILL_LV_DEFEND) 
            prop_school_skill_lv_spiritual = actor:GetProperty(PROP_SCHOOL_SKILL_LV_SPIRITUAL) 
            prop_school_skill_lv_speed = actor:GetProperty(PROP_SCHOOL_SKILL_LV_SPEED) 
            prop_school_skill_lv_dodge = actor:GetProperty(PROP_SCHOOL_SKILL_LV_DODGE) 

            prop_atk_practice_skill_lv	= actor:GetProperty(PROP_ATK_PRACTICE_SKILL_LV)
            prop_atk_resistance_skill_lv	= actor:GetProperty(PROP_ATK_RESISTANCE_SKILL_LV)
            prop_spell_practice_skill_lv	= actor:GetProperty(PROP_SPELL_PRACTICE_SKILL_LV)
            prop_spell_resistance_skill_lv	= actor:GetProperty(PROP_SPELL_RESISTANCE_SKILL_LV)
        end
    end)

    local actor = actor_manager_fetch_player_by_id(selected_actor_uid)
    if actor then
        if imgui.Button('SetLocal') then
            actor_manager_set_local_player(actor:GetID())
        end
        imgui.SameLine()

        if imgui.Button('创建召唤兽') then
            local player = actor_manager_fetch_local_player()
            local msg = {}
            msg.owner = player:GetID()
            msg.props = actor:GetProperties()

            net_send_message(PTO_C2S_CREATE_SUMMON, cjson.encode(msg)) 
        end
        imgui.SameLine()

        if imgui.Button('导出召唤兽') then
            net_manager_player_dostring([[
                summons_on_save()
            ]])
        end

        edit_prop_lv = actor:GetProperty(PROP_LV)
        imgui.PushItemWidth(80)
        res , edit_prop_lv =  imgui.DragInt('等级', edit_prop_lv)
        imgui.PopItemWidth()
        if res then
            net_manager_player_dostring(string.format([[ 
                local actor = actor_manager_fetch_player_by_id(%d)
                actor:SetProperty(PROP_LV, %d)
                actor:ClearAssignPoints()

                if actor:GetProperty(PROP_ACTOR_TYPE)==ACTOR_TYPE_SUMMON then
                    actor:ApplySummonQual('芙蓉仙子')
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
            if imgui.Button('门派') then
                imgui.OpenPopup('SchoolSelector')
            end
            imgui.SameLine()
            imgui.Text(actor:GetSchoolName())    
        end
        if imgui.BeginPopup('SchoolSelector') then
            local school = content_system_get_table('school')
            imgui.HorizontalLayout(school,next,function(k,v) 
                if imgui.Button(v.name..'##'..v.ID) then
                    net_manager_player_dostring(string.format([[ 
                        local actor = actor_manager_fetch_player_by_id(%d)
                        actor:SetProperty(PROP_SCHOOL, %d) 
                        ]],actor:GetID(), v.ID))
                    imgui.CloseCurrentPopup()
                end
            end)
            imgui.EndPopup('SchoolSelector')
        end

        if imgui.Button('满状态') then
            net_manager_player_dostring(string.format([[ 
                local actor = actor_manager_fetch_player_by_id(%d)
                actor:SetProperty(PROP_HP, actor:GetMaxHP()) 
                actor:SetProperty(PROP_MP, actor:GetMaxMP()) 
                actor:SetProperty(PROP_SP, 150) 
            ]], actor:GetID() ))
        end
        imgui.SameLine()
        imgui.Text(string.format('HP:%.f/%.f',actor:GetProperty(PROP_HP), actor:GetMaxHP()))
        imgui.SameLine()
        imgui.Text(string.format('MP:%.0f/%.0f',actor:GetProperty(PROP_MP),actor:GetMaxMP()))
        
        if actor:GetProperty(PROP_ACTOR_TYPE) ==  ACTOR_TYPE_PLAYER then
            imgui.SameLine()
            imgui.Text(string.format('SP:%.0f/%.0f',actor:GetProperty(PROP_SP), 150))
        end
        imgui.Separator()

        imgui.BeginGroup()
        imgui.Dummy(30,20)
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
        draw_prop_points_panel(actor, edit_prop_lv)
        imgui.Dummy(30,10)

        if actor:GetProperty(PROP_ACTOR_TYPE)  == ACTOR_TYPE_PLAYER then
            if imgui.CollapsingHeader('师门技能等级') then
                draw_player_skill_pannel(actor)
            end

            if imgui.CollapsingHeader('装备') then
                draw_player_equip_pannel(actor)
            end

            if imgui.CollapsingHeader('修炼等级') then
                draw_player_practice_lv(actor)
            end
        end
        if actor:GetProperty(PROP_ACTOR_TYPE) == ACTOR_TYPE_SUMMON then
            if imgui.CollapsingHeader('修炼等级') then
                draw_player_practice_lv(actor)
            end

            imgui.BeginGroup()
            imgui.Text('攻击资质 '..actor:GetProperty(PROP_SUMMON_ATK_QUAL))
            imgui.Text('防御资质 '..actor:GetProperty(PROP_SUMMON_DEF_QUAL))
            imgui.Text('体力资质 '..actor:GetProperty(PROP_SUMMON_HEALTH_QUAL))
            imgui.Text('法力资质 '..actor:GetProperty(PROP_SUMMON_MAGIC_QUAL))
            imgui.Text('速度资质 '..actor:GetProperty(PROP_SUMMON_SPEED_QUAL))
            imgui.Text('躲闪资质 '..actor:GetProperty(PROP_SUMMON_DODGE_QUAL))
            imgui.Text(string.format('成长 %.4f',  actor:GetProperty(PROP_SUMMON_GROW_COEF)))
            imgui.EndGroup()

            if imgui.Button('BB资质模板') then
                imgui.OpenPopup('BBQualSelector')
            end
            if imgui.BeginPopup('BBQualSelector') then
                local tbl = content_system_get_table('summon')
                imgui.HorizontalLayout(tbl,next,function(k,v) 
                    if imgui.Button(k..'##bb_templ') then
                        net_manager_player_dostring(string.format([[ 
                            local actor = actor_manager_fetch_player_by_id(%d)
                            actor:SetProperty(PROP_SUMMON_ATK_QUAL, %d)
                            actor:SetProperty(PROP_SUMMON_DEF_QUAL, %d)
                            actor:SetProperty(PROP_SUMMON_HEALTH_QUAL, %d)
                            actor:SetProperty(PROP_SUMMON_MAGIC_QUAL, %d)
                            actor:SetProperty(PROP_SUMMON_SPEED_QUAL, %d)
                            actor:SetProperty(PROP_SUMMON_DODGE_QUAL, %d)
                            actor:SetProperty(PROP_SUMMON_GROW_COEF, %f)
                        ]], actor:GetID(), v.atk_qual, v.def_qual, v.health_qual,v.magic_qual,v.speed_qual,v.dodge_qual,v.grow_coef))

                        imgui.CloseCurrentPopup()
                    end
                end)
                imgui.EndPopup('BBQualSelector')
            end
        end
    end
    imgui.End()
end