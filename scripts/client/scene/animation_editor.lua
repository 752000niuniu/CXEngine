local player 
local enemy
local added_frame = 0
local added_action = 0
local RoleIDSB = imgui.CreateStrbuf('0',256)
local WeaponIDSB = imgui.CreateStrbuf('3',256)
local LoadActionID = ACTION_IDLE
local TimeInterval = 0.016 * 4
local MagicAnim 
local BeHitAnim 
local magic_tsv 

--剑侠客 攻击
---待战-> Runto-> Attack-> Runback -> 待战
function OnAttackActionCapter(actor, avatar)
    local file = io.open(vfs_makepath('a.txt'), 'a+')
    file:write('ROLE:'..actor:GetRoleID()..' Cur:'..avatar:GetCurrentFrame()..' '..avatar:GetGroupFrameCount()..'\n')
    file:close()
end

function OnSceneInit()
    scene_set_map(1506)
    magic_tsv = content_system_get_table('magic')
    
    local ostime = os.time()
    enemy =  actor_manager_create_actor(ostime)
    enemy:SetProperties({
        [PROP_AVATAR_ID] = 'JMW-AXE',
        [PROP_WEAPON_AVATAR_ID] = 'JMW-AXE-060-X',
        [PROP_NAME] ='巨魔王',
        [PROP_RACE] = RACE_DEVIL,
        [PROP_POS] = {206 ,190},
        [PROP_BASE_HEALTH] =  333 ,
        [PROP_BASE_MAGIC] = 157 ,
        [PROP_BASE_FORCE] =  215,
        [PROP_BASE_STAMINA] = 157  ,
        [PROP_BASE_AGILITY] =  689,
        [PROP_LV] =  145
    })

    enemy:SetProperty(PROP_HP, enemy:GetMaxHP()/3)
    enemy:SetProperty(PROP_MP, enemy:GetMaxMP())
    
    -- enemy:ChangePalMatrix(get_pal_from_json('{"1":{"to":40,"mat":[105,273,0,512,0,359,459,412,464],"from":0},"2":{"to":60,"mat":[86,96,134,244,301,144,273,14,330],"from":40},"3":{"to":120,"mat":[24,234,340,325,421,483,345,340,330],"from":60},"4":{"to":256,"mat":[255,0,0,0,255,0,0,0,255],"from":120},"segments":[0,40,60,120,256]}'))
    enemy:SetActionID(ACTION_BATIDLE)

    local enemys = {}
    for i=1,3 do
        ostime = ostime+1
        local actor =  actor_manager_create_actor(ostime)
        actor:SetProperties({
            [PROP_AVATAR_ID] = 'JMW-AXE',
            [PROP_WEAPON_AVATAR_ID] = 'JMW-AXE-060-X',
            [PROP_NAME] ='巨魔王'..i,
            [PROP_RACE] = RACE_DEVIL,
            [PROP_POS] = {206 ,190},
            [PROP_BASE_HEALTH] =  333 ,
            [PROP_BASE_MAGIC] = 157 ,
            [PROP_BASE_FORCE] =  215,
            [PROP_BASE_STAMINA] = 157  ,
            [PROP_BASE_AGILITY] =  689,
            [PROP_SCHOOL] =  SCHOOL_DT,
            [PROP_LV] =  145
        })

        actor:SetProperty(PROP_HP, enemy:GetMaxHP()/2)
        actor:SetProperty(PROP_MP, enemy:GetMaxMP())
        actor:SetActionID(ACTION_BATIDLE)
        table.insert(enemys,actor)
    end

    ostime = ostime + 1
    enemyBB  = actor_manager_create_actor(ostime)
    enemyBB:SetProperties({
        [PROP_ACTOR_TYPE] = ACTOR_TYPE_SUMMON,
        [PROP_AVATAR_ID] = '古代瑞兽',
        [PROP_WEAPON_AVATAR_ID] = '',
        [PROP_NAME] ='古代瑞兽',
        [PROP_POS] = {274, 250}
    })

    -- enemyBB:ChangePalMatrix(get_pal_from_json('{"1":{"to":256,"mat":[52,0,0,88,254,204,161,204,154],"from":0},"segments":[0,256]}')  )
    enemyBB:SetActionID(ACTION_BATIDLE)
    
    ostime = ostime + 1
    -- playerBB  = actor_manager_create_actor(ostime)
    -- playerBB:SetProperties({
    --     [PROP_ACTOR_TYPE] = ACTOR_TYPE_SUMMON,
    --     [PROP_AVATAR_ID] = '鬼将',
    --     [PROP_WEAPON_AVATAR_ID] = '',
    --     [PROP_NAME] ='鬼将',
    --     [PROP_POS] = {563,376},
    --     [PROP_SUMMON_ATK_QUAL] = 1400,
    --     [PROP_SUMMON_DEF_QUAL] = 1400,
    --     [PROP_SUMMON_HEALTH_QUAL] = 4500,
    --     [PROP_SUMMON_MAGIC_QUAL] =  2500,
    --     [PROP_SUMMON_SPEED_QUAL] = 1200,
    --     [PROP_SUMMON_DODGE_QUAL] =  1200,
    --     [PROP_SUMMON_GROW_COEF] =  1.25,
    --     [PROP_BASE_HEALTH] = 417 ,
    --     [PROP_BASE_MAGIC] = 680 ,
    --     [PROP_BASE_FORCE] = 171,
    --     [PROP_BASE_STAMINA] =  171 ,
    --     [PROP_BASE_AGILITY] = 171,
    --     [PROP_LV] = 151
    -- })
    -- playerBB:SetProperty(PROP_HP, playerBB:GetMaxHP())
    -- playerBB:SetProperty(PROP_MP, playerBB:GetMaxMP())
    

    -- -- playerBB:ChangePalMatrix(get_pal_from_json('{"1":{"to":256,"mat":[115,115,110,163,256,239,196,91,292],"from":0},"segments":[0,256]}'))
    -- playerBB:SetActionID(ACTION_BATIDLE)

    ostime = ostime + 1
    player = actor_manager_create_actor(ostime)
    player:SetProperties({
        [PROP_AVATAR_ID] = 'JXK-SWORD',
        [PROP_WEAPON_AVATAR_ID] = 'JXK-SWORD-060-X',
        [PROP_NAME] ='剑侠客',
        [PROP_RACE] = RACE_HUMAN,
        [PROP_POS] = {616,412},
        [PROP_BASE_HEALTH] =  333 ,
        [PROP_BASE_MAGIC] = 157 ,
        [PROP_BASE_FORCE] =  689,
        [PROP_BASE_STAMINA] = 215 ,
        [PROP_BASE_AGILITY] =  157,
        [PROP_SCHOOL] =  SCHOOL_DT,
        [PROP_LV] =  145
    })

    player:SetProperty(PROP_HP, player:GetMaxHP()/3)
    player:SetProperty(PROP_MP, player:GetMaxMP())
    
    local new_pal = get_pal_from_json('{"1":{"mat":[220,220,512,512,512,510,512,510,512],"to":40,"from":0},"2":{"mat":[211,211,230,445,292,445,306,306,206],"to":80,"from":40},"3":{"mat":[0,0,0,24,24,24,19,19,19],"to":120,"from":80},"4":{"mat":[255,0,0,0,255,0,0,0,255],"to":256,"from":120},"segments":[0,40,80,120,256]}')  
    -- player:ChangePalMatrix(new_pal)
    player:SetActionID(ACTION_BATIDLE)
    
    -- actor_manager_set_local_player(player:GetID())
    player:SetDir(0)
    player:ReverseDir()

    local actors = actor_manager_fetch_all_players()
    for i,actor in ipairs(actors) do
       actor:SetProperty(PROP_SCENE_ID,-105)
    end
    player:SetTarget(enemy)
    player:StopMove()
    -- {player:GetTarget(),playerBB,enemyBB}
    
    
    -- combat_system_start_battle({player},enemys)


    cxlog_info(player:GetProperty(PROP_NAME), player:GetTarget():GetProperty(PROP_NAME))

    
	local msg = {}
	msg.account = 'oceancx'
	msg.password = '123456'
	msg.name = 'Ocean藏心'
	msg.scene_id = -105
	msg.role_id = 1
	msg.weapon_id = 40
	msg.x = 306
	msg.y = 466
	net_send_message(PTO_C2C_LOGIN, cjson.encode(msg))
end

local actor_dir = 0
local actor_action = 0
local actor_frame = 0
local actor_show_boudingbox={}
local menu_show = true
local bb_menu_show = true
local magic_menu_show = false

function OnSceneImGuiUpdate()
    if imgui.Button('Close') then
        npc_dialog_show(false,'')
    end
end

function check_dest_hit_actor(dest_x, dest_y)
    local hit_actor = nil
    local actors = actor_manager_fetch_all_players()
    for i,actor in ipairs(actors) do
        cxlog_info('check_dest_hit_actor', actor:GetProperty(PROP_NAME))
        if not actor:IsLocal() then
            local avatar = actor:GetAvatar()
            if not avatar then return end
            local avx, avy = actor:GetPos()
            local cx =  avx - avatar:GetFrameKeyX()
            local cy =  avy - avatar:GetFrameKeyY()
            local brx = avatar:GetFrameWidth()
            local bry =  avatar:GetFrameHeight()
            if dest_x >= cx and dest_x <= cx+brx and dest_y >= cy and dest_y <=cy+bry  then
                hit_actor = actor
                break
            end
        end
        
    end
    return hit_actor
end

function OnSceneUpdate()
    if scene_is_combat() then
        
    else
        if imgui.IsMouseClicked(0) then
            
            local player = actor_manager_fetch_local_player()
            if not player then return end
            local mx,my = input_manager_get_mouse_pos()
            local dest_x, dest_y = util_screen_pos_to_map_pos(mx, my)
            local hit_actor = check_dest_hit_actor(dest_x,dest_y)
            if not hit_actor and not npc_dialog_is_show()  then
                player:MoveTo(dest_x,dest_y)
                local msg = {}
                msg.pid = player:GetID()
                msg.x = dest_x
                msg.y = dest_y
                net_send_message(PTO_C2C_MOVE_TO_POS, cjson.encode(msg))
            else
           
            end
        end
    end
end

function OnSceneDraw()
    if scene_is_combat() then

    else    

    end
end


