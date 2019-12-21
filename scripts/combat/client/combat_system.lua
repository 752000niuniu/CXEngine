
local CommandMT = {}
function CommandMT:new(o)
    o = o or {}
    self.__index = self 
    setmetatable(o, self)
    return o
end

function CommandMT:Init(battle, actor, skill_id)
    self.battle = battle
    self.state = COMMAND_STATE_PREPARE
    self.type =  COMMAND_TYPE_ATTACK
    self.skill_id = skill_id
    self.master = actor
    self.main_target = actor:GetTarget()
    self.targets = { self.main_target }
    self.acting_actors = {}
end

function CommandMT:AddTarget(actor)
    table.insert(self.targets, actor)
end

function CommandMT:SetMainTarget(actor)
    for i, target in ipairs(self.targets) do
        if target == actor then
            self.main_target = actor
            return 
        end
    end
    self:AddTarget(actor)
    self.main_target = actor
end

function CommandMT:GetState()
    return self.state
end

function CommandMT:GetMaster()
    return self.master
end

function CommandMT:GetType()
    return self.type
end

function CommandMT:AddActing(actor)
    for i,v in ipairs(self.acting_actors) do
        if v:GetID() == actor:GetID() then
            return
        end
    end
    table.insert(self.acting_actors,actor)
end

function CommandMT:RemoveActing(actor)
    for i,v in ipairs(self.acting_actors) do
        if v:GetID() == actor:GetID() then
            table.remove(self.acting_actors, i)
            if #self.acting_actors == 0 then
                self.state = COMMAND_STATE_STOP
            end
            return 
        end
    end
end

function CommandMT:IsActing()
    return #self.acting_actors > 0
end

function CommandMT:StartCast()
    if self.battle then
        local actor = self.master
        actor:CastSkill(self.skill_id,self.battle.turn)
    end
end

function CommandMT:Update()
    if self.state == COMMAND_STATE_PREPARE then
        if self.master:IsDead() then
            self.state = COMMAND_STATE_STOP
            return
        end
        self:StartCast()
        self.state = COMMAND_STATE_PALY
    elseif self.state == COMMAND_STATE_PALY then
        
    elseif self.state == COMMAND_STATE_STOP then
        
    end    
end

function CommandMT:IsFinished()
    return self.state == COMMAND_STATE_STOP
end

local ACTOR_CLICK_MODE_ATTACK = 1
local ACTOR_CLICK_MODE_SPELL = 2
local ACTOR_CLICK_MODE  = ACTOR_CLICK_MODE_ATTACK

local battle_commands = {}

local skill_template_table  = {}
local buffer_template_table = {}
local battle    
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
    skill_template_table = content_system_get_table('skill')
    buffer_template_table = content_system_get_table('buffer')
    -- cxlog_info('combat_system_init',cjson.encode(skill_template_table))
    init_skills()
    init_buffers()
end

function combat_system_current_turn()
    if battle then
        return battle.turn
    else
        return 0
    end
end    

function combat_system_current_cmd()
    return battle_commands[1]
end

function combat_system_actor_on_click(actor, button, x, y)
    if not battle or battle.state ~= BATTLE_TURN_STAND_BY then return end
    
    local player = actor_manager_fetch_local_player()
    if player==actor then return end
    player:SetTarget(actor)

    if  ACTOR_CLICK_MODE == ACTOR_CLICK_MODE_ATTACK then
        player:SetProperty(PROP_USING_SKILL,1)
    end

    local msg = {}
    msg.type = 'ATK'
    msg.master = player:GetID()
    msg.target = actor:GetID()
    msg.skill_id = player:GetProperty(PROP_USING_SKILL)
    net_send_message(PTO_C2S_COMBAT_CMD_PVP, cjson.encode(msg) )

    
    ACTOR_CLICK_MODE = ACTOR_CLICK_MODE_ATTACK
end


local actor_on_click_cb = {}
function combat_reset_actor(actor)
    actor:SetProperty(PROP_ASM_BUFF_ANIM, 0)
    -- actor:SetActionID(ACTION_IDLE)
    actor:SetProperty(PROP_IS_COMBAT,false)
    actor:SetProperty(PROP_CAN_MOVE,true)
    actor:ClearAction()
    actor:ClearBackAnim()
    actor:ClearFrontAnim()
    actor:PushAction(ACTION_IDLE)
    local avatar = actor:GetAvatar()
    actor_reg_event(actor, ACTOR_EV_ON_CLICK, actor_on_click_cb[actor:GetID()])
end


function check_battle_end()
    if not battle then return true end
    local atk_all_dead = true
    local def_all_dead = true
    for i,actor in ipairs(battle.actors) do
        if not actor:IsDead() then
            if actor:GetProperty(PROP_TEAM_TYPE) == TEAM_TYPE_ATTACKER then
                atk_all_dead = false
            else
                def_all_dead = false
            end
        end
    end
    return atk_all_dead or def_all_dead
end


function combat_system_draw()
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
        on_battle_end(battle)
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

stub[PTO_S2C_COMBAT_START] = function(req)
    local atk = actor_manager_fetch_player_by_id(req.atk)
    local def = actor_manager_fetch_player_by_id(req.def)
    atk:SetProperty(PROP_HP, req.atk_hp)
    def:SetProperty(PROP_HP, req.def_hp)
    
    local player = actor_manager_fetch_local_player()
    if player:GetID() == atk:GetID() or def:GetID() == player:GetID() then
        scene_set_combat(true)
        battle = BattleMT:new()
        
        local team_id = os.time()
        for i,actor in ipairs({atk}) do
            table.insert(battle.actors, actor)
        end

        team_id = team_id + 1
        for i,actor in ipairs({def}) do
            table.insert(battle.actors, actor)
        end
        battle.state = BATTLE_START
    end
end 

stub[PTO_S2C_COMBAT_EXECUTE] = function(req)
    battle.state = BTTALE_TURN_EXECUTE 

    for i,req_cmd in ipairs(req.cmds) do
        local actor = actor_manager_fetch_player_by_id(req_cmd.master)
        local target = actor_manager_fetch_player_by_id(req_cmd.target)
        actor:SetTarget(target)
        local cmd = CommandMT:new()
        cmd:Init(battle,actor,req_cmd.skill_id )
        table.insert(battle_commands,cmd)
    end
end


function on_battle_start(self)
    cxlog_info('on_battle_start')
    ui_renderer_clear()
    local init_actor = function(actor, pos, dir)
        actor:SetCombatPos(pos.x,pos.y)
        actor:SetDir(dir)
        actor:ClearAction()
        
        actor:PushAction(ACTION_BATIDLE)

        actor_on_click_cb[actor:GetID()] = actor_reg_event(actor, ACTOR_EV_ON_CLICK, function(actor, button, x, y)
            combat_system_actor_on_click(actor,button,x,y)
        end)
    end

    local player = actor_manager_fetch_local_player()
    local self_team_type = player:GetProperty(PROP_TEAM_TYPE)
    for i,actor in ipairs(self.actors) do
        if actor:GetProperty(PROP_TEAM_TYPE) == self_team_type then
            local pos = combat_self_pos[i]
            init_actor(actor, pos, DIR_NW)
        else
            local pos =  combat_enemy_pos[i]
            init_actor(actor, pos, DIR_SE)
        end
    end
    battle.state = BATTLE_TURN_STAND_BY 
    cxlog_info('BATTLE_TURN_STAND_BY')
    -- skill_on_turn(CurrentTurn)
end

function on_battle_turn_stand_by(self)
    --[[
    每帧update都看player是不是输入了指令
    local player的指令输入来自键盘 或者  鼠标
    而 npc的输入来自随机
    敌对玩家的输入来自网络
]]
end

function on_battle_turn_execute(self)
    --[[
    处理每一条战斗指令,
    每条战斗指令,代表某个actor的演出
    处理完战斗指令,所有的actor演出结束
    战斗指令的先后顺序代表actor的先后顺序

    攻击指令
        攻击对象 攻击效果
    施法指令
        
    防御指令
]]
        
    if #battle_commands > 0 then
        local cmd = battle_commands[1]
        if not cmd.master:IsDead() then
            cmd:Update()
            if cmd:IsFinished() then
                table.remove(battle_commands,1)
            end
        else
            table.remove(battle_commands,1)
        end
    else 
        if check_battle_end() then
            self.state = BATTLE_END
            cxlog_info('BATTLE_END')
        else
            self.state = BTTALE_TURN_NEXT
            cxlog_info('BTTALE_TURN_NEXT')
        end
    end
end

function on_battle_turn_next(self)
    cxlog_info('on_battle_turn_next')
    self.turn = self.turn + 1
    -- for i,actor in ipairs(self.actors) do
    --     actor:BufferNextTurn(self.turn)
    -- end
    self.state = BATTLE_TURN_STAND_BY
end

function on_battle_end(self)
     cxlog_info('on_battle_end')
    for i,actor in ipairs(self.actors) do
        combat_reset_actor(actor)
    end
    
    animation_manager_clear()
    scene_set_combat(false)
    self.state = BATTLE_DEFAULT
end

function combat_system_update()
    if battle then
        battle:Update()
        
        for i,actor in ipairs(battle.actors) do
            actor:Update()
        end
        animation_manager_update()
    end
end

function combat_system_remove_from_battle(_actor_)
    if not battle then return end
    for i,actor in ipairs(battle.actors) do
        if actor:GetID() == _actor_:GetID() then
            combat_reset_actor(actor)
            table.remove(battle.actors,i) 
            return
        end
    end
end
