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
    -- enemy =  actor_manager_create_actor(ostime)
    -- enemy:SetProperties({
    --     [PROP_AVATAR_ID] = 'JMW-AXE',
    --     [PROP_WEAPON_AVATAR_ID] = 'JMW-AXE-060-X',
    --     [PROP_NAME] ='巨魔王',
    --     [PROP_RACE] = RACE_DEVIL,
    --     [PROP_POS] = {206 ,190},
    --     [PROP_BASE_HEALTH] =  333 ,
    --     [PROP_BASE_MAGIC] = 157 ,
    --     [PROP_BASE_FORCE] =  215,
    --     [PROP_BASE_STAMINA] = 157  ,
    --     [PROP_BASE_AGILITY] =  689,
    --     [PROP_LV] =  145
    -- })

    -- enemy:SetProperty(PROP_HP, enemy:GetMaxHP()/3)
    -- enemy:SetProperty(PROP_MP, enemy:GetMaxMP())
    
    -- -- enemy:ChangePalMatrix(get_pal_from_json('{"1":{"to":40,"mat":[105,273,0,512,0,359,459,412,464],"from":0},"2":{"to":60,"mat":[86,96,134,244,301,144,273,14,330],"from":40},"3":{"to":120,"mat":[24,234,340,325,421,483,345,340,330],"from":60},"4":{"to":256,"mat":[255,0,0,0,255,0,0,0,255],"from":120},"segments":[0,40,60,120,256]}'))
    -- enemy:SetActionID(ACTION_BATIDLE)


    -- player = actor_manager_create_actor(ostime)
    -- player:SetProperties({
    --     [PROP_AVATAR_ID] = 'JXK-SWORD',
    --     [PROP_WEAPON_AVATAR_ID] = 'JXK-SWORD-060-X',
    --     [PROP_NAME] ='剑侠客',
    --     [PROP_RACE] = RACE_HUMAN,
    --     [PROP_POS] = {616,412},
    --     [PROP_BASE_HEALTH] =  333 ,
    --     [PROP_BASE_MAGIC] = 157 ,
    --     [PROP_BASE_FORCE] =  689,
    --     [PROP_BASE_STAMINA] = 215 ,
    --     [PROP_BASE_AGILITY] =  157,
    --     [PROP_SCHOOL] =  SCHOOL_DT,
    --     [PROP_LV] =  145
    -- })

    -- player:SetProperty(PROP_HP, player:GetMaxHP()/3)
    -- player:SetProperty(PROP_MP, player:GetMaxMP())
    
    -- local new_pal = get_pal_from_json('{"1":{"mat":[220,220,512,512,512,510,512,510,512],"to":40,"from":0},"2":{"mat":[211,211,230,445,292,445,306,306,206],"to":80,"from":40},"3":{"mat":[0,0,0,24,24,24,19,19,19],"to":120,"from":80},"4":{"mat":[255,0,0,0,255,0,0,0,255],"to":256,"from":120},"segments":[0,40,80,120,256]}')  
    -- -- player:ChangePalMatrix(new_pal)
    -- player:SetActionID(ACTION_BATIDLE)
    
    -- -- actor_manager_set_local_player(player:GetID())
    -- player:SetDir(0)
    -- player:ReverseDir()
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


function OnSceneUpdate()
    if scene_is_combat() then
        
    else
        

        
    end
end

function OnSceneDraw()
    if scene_is_combat() then

    else    

    end
end


