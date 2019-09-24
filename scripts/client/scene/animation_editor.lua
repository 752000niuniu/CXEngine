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
        [PROP_POS] = {206 ,190}
    })
    -- enemy:ChangePalMatrix(get_pal_from_json('{"1":{"to":40,"mat":[105,273,0,512,0,359,459,412,464],"from":0},"2":{"to":60,"mat":[86,96,134,244,301,144,273,14,330],"from":40},"3":{"to":120,"mat":[24,234,340,325,421,483,345,340,330],"from":60},"4":{"to":256,"mat":[255,0,0,0,255,0,0,0,255],"from":120},"segments":[0,40,60,120,256]}'))
    enemy:SetActionID(ACTION_BATIDLE)

    ostime = ostime + 1
    enemyBB  = actor_manager_create_actor(ostime)
    enemyBB:SetProperties({
        [PROP_ACTOR_TYPE] = ACTOR_TYPE_PET,
        [PROP_AVATAR_ID] = '古代瑞兽',
        [PROP_WEAPON_AVATAR_ID] = '',
        [PROP_NAME] ='古代瑞兽',
        [PROP_POS] = {274, 250}
    })
    -- enemyBB:ChangePalMatrix(get_pal_from_json('{"1":{"to":256,"mat":[52,0,0,88,254,204,161,204,154],"from":0},"segments":[0,256]}')  )
    enemyBB:SetActionID(ACTION_BATIDLE)
    
    ostime = ostime + 1
    playerBB  = actor_manager_create_actor(ostime)
    playerBB:SetProperties({
        [PROP_ACTOR_TYPE] = ACTOR_TYPE_PET,
        [PROP_AVATAR_ID] = '鬼将',
        [PROP_WEAPON_AVATAR_ID] = '',
        [PROP_NAME] ='鬼将',
        [PROP_POS] = {563,376}
    })
    -- playerBB:ChangePalMatrix(get_pal_from_json('{"1":{"to":256,"mat":[115,115,110,163,256,239,196,91,292],"from":0},"segments":[0,256]}'))
    playerBB:SetActionID(ACTION_BATIDLE)

    ostime = ostime + 1
    player = actor_manager_create_actor(ostime)
    player:SetProperties({
        [PROP_AVATAR_ID] = 'JXK-SWORD',
        [PROP_WEAPON_AVATAR_ID] = 'JXK-SWORD-060-X',
        [PROP_NAME] ='剑侠客',
        [PROP_POS] = {616,412}
    })
    local new_pal = get_pal_from_json('{"1":{"mat":[220,220,512,512,512,510,512,510,512],"to":40,"from":0},"2":{"mat":[211,211,230,445,292,445,306,306,206],"to":80,"from":40},"3":{"mat":[0,0,0,24,24,24,19,19,19],"to":120,"from":80},"4":{"mat":[255,0,0,0,255,0,0,0,255],"to":256,"from":120},"segments":[0,40,80,120,256]}')  
    -- player:ChangePalMatrix(new_pal)
    player:SetActionID(ACTION_BATIDLE)
    
    actor_manager_set_local_player(player:GetID())
    player:SetDir(0)
    player:ReverseDir()

    -- playerBB:SetDir(player:GetDir())
    -- enemy:SetDir(player:GetDir())
    -- enemy:ReverseDir()

    -- enemyBB:SetDir(enemy:GetDir())
    
    -- player:SetTarget(enemy)
    -- enemy:SetTarget(player)

    -- playerBB:SetTarget(enemyBB)
    -- enemyBB:SetTarget(playerBB)

    local actors = actor_manager_fetch_all_players()
    for i,actor in ipairs(actors) do
        actor:SetProperty(PROP_SCENE_ID,-105)
    end
end

local actor_dir = 0
local actor_action = 0
local actor_frame = 0
local actor_show_boudingbox={}
local menu_show = true
local bb_menu_show = true
local magic_menu_show = false

function OnSceneImGuiUpdate()
 
end

function check_dest_hit_actor(dest_x, dest_y)
    local hit_actor = nil
    local actors = actor_manager_fetch_all_players()
    for i,actor in ipairs(actors) do
        if not actor:IsLocal() then
            local avatar = actor:GetAvatar()
            local avx, avy = actor:GetProperty(PROP_POS)
            local cx =  avx - avatar:GetFrameKeyX()
            local cy =  avy - avatar:GetFrameKeyY()
            local brx = avatar:GetFrameWidth()
            local bry =  avatar:GetFrameHeight()
            if dest_x >= cx and dest_x <= cx+brx and dest_y >= cy and dest_y <=cy+bry  then
                hit_actor = actor
                break
            end
        end
        -- cxlog_info(mx, my, cx,cy ,brx,bry) 
    end
    return hit_actor
end

function OnSceneUpdate()
    if scene_is_combat() then
        
    else
        if imgui.IsMouseClicked(0) then
            local mx,my = input_manager_get_mouse_pos()
            local dest_x, dest_y = util_screen_pos_to_map_pos(mx, my)
            local hit_actor = check_dest_hit_actor(dest_x,dest_y)
            if not hit_actor then
                player:MoveTo(dest_x,dest_y)
                local msg = {}
                msg.pid = player:GetID()
                msg.x = dest_x
                msg.y = dest_y
                net_send_message(PTO_C2C_MOVE_TO_POS, cjson.encode(msg))
            else
                hit_actor:Say('神州上下祸劫频生，灵石是否重补苍天裂痕，蚩尤的阴谋能否被挫败，三界将继续下去还是回归混沌，将由玩家英雄亲手创造。')
            end
        end
    end
end

function OnSceneDraw()
    if scene_is_combat() then

    else    

    end
end


