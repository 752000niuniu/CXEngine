__battles__ = __battles__ or {}

local ActorMT = actor_get_metatable()

function ActorMT:GetBattle()
	local battle_id = self:GetProperty(PROP_COMBAT_BATTLE_ID)
	return __battles__[battle_id]
end

local battle_commands = {}

local ACTOR_CLICK_MODE_ATTACK = 1
local ACTOR_CLICK_MODE_SPELL = 2
local ACTOR_CLICK_MODE  = ACTOR_CLICK_MODE_ATTACK


BattleBG = BattleBG or nil
combat_self_pos = combat_self_pos or {}
combat_enemy_pos = combat_enemy_pos or {}
function calc_combat_self_pos(ratio_x, ratio_y)
	return {
		{ x = 415.0 * ratio_x, y = 275.0 * ratio_y },
		{ x = 355.0 * ratio_x, y = 305.0 * ratio_y },
		{ x = 475.0 * ratio_x, y = 245.0 * ratio_y },
		{ x = 295.0 * ratio_x, y = 335.0 * ratio_y },
		{ x = 535.0 * ratio_x, y = 215.0 * ratio_y },
		{ x = 465.0 * ratio_x, y = 315.0 * ratio_y },
		{ x = 405.0 * ratio_x, y = 345.0 * ratio_y },
		{ x = 525.0 * ratio_x, y = 285.0 * ratio_y },
		{ x = 345.0 * ratio_x, y = 375.0 * ratio_y },
		{ x = 585.0 * ratio_x, y = 255.0 * ratio_y },
	}
end

function calc_combat_enemy_pos(ratio_x, ratio_y)
	return {
		{ x = 175.0 * ratio_x, y = 170.0 * ratio_y },   
		{ x = 115.0 * ratio_x, y = 200.0 * ratio_y },   
		{ x = 235.0 * ratio_x, y = 140.0 * ratio_y },   
		{ x = 55.0  * ratio_x, y = 230.0 * ratio_y },  
		{ x = 295.0 * ratio_x, y = 110.0 * ratio_y },   
		{ x = 220.0 * ratio_x, y = 210.0 * ratio_y },   
		{ x = 160.0 * ratio_x, y = 240.0 * ratio_y },   
		{ x = 280.0 * ratio_x, y = 180.0 * ratio_y },   
		{ x = 100.0 * ratio_x, y = 270.0 * ratio_y },   
		{ x = 340.0 * ratio_x, y = 150.0 * ratio_y },   
	}
end

function combat_system_init()
    BattleBG = animation_create(ADDONWDF, 0xE3B87E0F)
    local ratio_x = game_get_width()/ 640 
	local ratio_y = game_get_height()/ 480
    combat_self_pos =  calc_combat_self_pos(ratio_x, ratio_y)
    combat_enemy_pos =  calc_combat_enemy_pos(ratio_x, ratio_y)
    init_skills()
    init_buffers()
end

function combat_system_current_cmd()
    return battle_commands[1]
end

function combat_system_actor_ev_on_click(actor, button, x, y)
    local player = actor_manager_fetch_local_player()
    local battle = player:GetBattle()
    if not battle or battle.state ~= BATTLE_TURN_STAND_BY then return end
    
    if not battle:InBattle(actor) then return end

    local player = actor_manager_fetch_local_player()
    player:SetTarget(actor)

    if ACTOR_CLICK_MODE == ACTOR_CLICK_MODE_ATTACK then
        player:SetProperty(PROP_USING_SKILL,1)
    end

    local msg = {}
    msg.type = 'ATK'
    msg.master = player:GetID()
    msg.target = actor:GetID()
    msg.skill_id = player:GetProperty(PROP_USING_SKILL)
    net_send_message(PTO_C2S_COMBAT_CMD, cjson.encode(msg) )

    ACTOR_CLICK_MODE = ACTOR_CLICK_MODE_ATTACK
end

function combat_reset_actor(actor)
    actor:SetProperty(PROP_ASM_BUFF_ANIM, 0)
    actor:SetProperty(PROP_IS_COMBAT,false)
    actor:SetProperty(PROP_CAN_MOVE,true)
    actor:ClearAction()
    actor:ClearBackAnim()
    actor:ClearFrontAnim()
    actor:PushAction(ACTION_IDLE)    
end

function combat_system_draw()
    local player = actor_manager_fetch_local_player()
    local battle = player:GetBattle()
    if not battle then return end
    BattleBG:Draw()

    local drawActors = {}
    for i,actor in ipairs(battle.actors) do
        table.insert(drawActors,actor)
    end
    table.sort(drawActors,function(a,b) return a:GetY() < b:GetY() end)

    for i,actor in ipairs(drawActors) do
        actor:Draw()
    end
    animation_manager_draw()
end


function combat_system_imgui_update()
    local player = actor_manager_fetch_local_player()
    local battle = player:GetBattle()
    if not battle or battle.state ~= BATTLE_TURN_STAND_BY then return end

	imgui.Begin('Menu',menu_show)

    if imgui.Button('攻击##player') then
        
    end  

    if imgui.Button('法术##player') then
        imgui.OpenPopup('SpellSelector')
        ACTOR_CLICK_MODE = ACTOR_CLICK_MODE_SPELL
    end

    if imgui.Button('特技##player') then

    end

    if imgui.Button('道具##player') then

    end

    if imgui.Button('防御##player') then

    end

    if imgui.Button('召唤##player') then

    end

    if imgui.Button('召还##player') then

    end

    if imgui.Button('捕捉##player') then

    end

    if imgui.Button('逃跑##player') then
        
        -- battle:EndBattle()
    end

    imgui.SetNextWindowSize(350,400)
    if imgui.BeginPopup('SpellSelector') then
        local skill_tbl = content_system_get_table('skill')
        local player = actor_manager_fetch_local_player()      
        local school = player:GetProperty(PROP_SCHOOL)
        local school_skill = {}
        for id,row in pairs(skill_tbl) do
            if row.school == school then
                school_skill[id] = row
            end
        end
        imgui.HorizontalLayout(school_skill,next,function(k,v) 
            if imgui.Button(v.name..'##'..v.ID) then
                player:SetProperty(PROP_USING_SKILL, v.ID)
                imgui.CloseCurrentPopup()
            end
        end)
        imgui.EndPopup('SpellSelector')
    end
    imgui.End()
    -- imgui.Begin('BBMenu',bb_menu_show)
    -- if imgui.Button('法术##bb') then

    -- end

    -- if imgui.Button('道具##bb') then

    -- end

    -- if imgui.Button('防御##bb') then

    -- end

    -- if imgui.Button('逃跑##bb') then

    -- end
    -- imgui.End()
    for i,actor in ipairs(battle.actors) do
        local x,y,w,h = actor:GetAvatarRect()
        imgui.SetCursorPos(x,y-10)
        local max_hp = actor:GetMaxHP()
        local hp = actor:GetHP()
        imgui.Text(hp..'/'..max_hp)
    end
end

local stub = net_manager_stub()
stub[PTO_S2C_COMBAT_START] = function(resp)
    local battle = BattleMT:new()
    battle:Deserialize(resp.battle)
    __battles__[battle.id] = battle
    
    local player = actor_manager_fetch_local_player()
    for i, actor in ipairs(battle.actors) do
        if player:GetID() == actor:GetID() then
            battle:StartBattle()
            return
        end
    end
end 

stub[PTO_S2C_COMBAT_EXECUTE] = function(req)
    local player = actor_manager_fetch_local_player()
    local battle = player:GetBattle()
    for i,req_cmd in ipairs(req.cmds) do
        local actor = actor_manager_fetch_player_by_id(req_cmd.master)
        local target = actor_manager_fetch_player_by_id(req_cmd.target)
        actor:SetTarget(target)
        table.insert(battle_commands,req_cmd)
    end
    battle.state = BTTALE_TURN_EXECUTE 
end

function on_battle_start(self)
    ui_renderer_clear()
    local init_actor = function(actor, pos, dir)
        actor:SetCombatPos(pos.x,pos.y)
        actor:SetDir(dir)
        actor:ClearAction()
        actor:SetProperty(PROP_IS_COMBAT,true)
        actor:PushAction(ACTION_BATIDLE)
    end

    local player = actor_manager_fetch_local_player()
    local self_team_type = player:GetProperty(PROP_TEAM_TYPE)
    local self_pos_i = 1
    local enemy_pos_i = 1
    for i,actor in ipairs(self.actors) do
        if actor:GetProperty(PROP_TEAM_TYPE) == self_team_type then
            local pos = combat_self_pos[self_pos_i]
            self_pos_i = self_pos_i + 1
            init_actor(actor, pos, DIR_NW)
        else
            local pos =  combat_enemy_pos[enemy_pos_i]
            enemy_pos_i = enemy_pos_i + 1
            init_actor(actor, pos, DIR_SE)
        end
    end
end

function combat_system_update()
    local player = actor_manager_fetch_local_player()
    local battle = player:GetBattle()
    if battle then
        if battle.state == BTTALE_TURN_EXECUTE then
            if #battle_commands > 0 then
                local cmd = battle_commands[1]
                local actor = actor_manager_fetch_player_by_id(cmd.master)
                local skill = actor:GetUsingSkill()
                if not skill then
                    if battle:InBattle(actor) then
                        local target = actor_manager_fetch_player_by_id(cmd.target)
                        actor:CastSkill(cmd.skill_id) 
                    else
                        table.remove(battle_commands,1)    
                    end
                else 
                    cxlog_info('skill ', skill.id , skill.caster_end, skill.target_end, skill.tid, cjson.encode(cmd))
                    if skill.caster_end and skill.target_end then
                        actor:EndUsingSkill()
                        table.remove(battle_commands,1)
                    end
                end
            else 
                if battle:CheckEnd() then
                    battle:EndBattle()
                    return
                else
                    battle:NextTurn()
                end
            end
        end

        for i,actor in ipairs(battle.actors) do
            actor:Update()
        end
        animation_manager_update()
    end
end

function combat_system_remove_from_battle(_actor_)
    local player = actor_manager_fetch_local_player()
    local battle = player:GetBattle()
    if not battle then return end
    for i,actor in ipairs(battle.actors) do
        if actor:GetID() == _actor_:GetID() then
            combat_reset_actor(actor)
            table.remove(battle.actors,i) 
            return
        end
    end
end

stub[PTO_S2C_COMBAT_LEAVE_BATTLE] = function(resp)
end 