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
local BattleBG

--剑侠客 攻击
---待战-> Runto-> Attack-> Runback -> 待战
function OnAttackActionCapter(actor, avatar)
    local file = io.open(vfs_makepath('a.txt'), 'a+')
    file:write('ROLE:'..actor:GetRoleID()..' Cur:'..avatar:GetCurrentFrame()..' '..avatar:GetGroupFrameCount()..'\n')
    file:close()
end

function get_pal_from_json(str)
    local pal = cjson.decode(str)
    local new_pal = {}
    for k,v in pairs(pal) do
        if k~='segments' then
            table.insert(new_pal,v)
        else
            new_pal.segments = v
        end
    end
    return new_pal
end

function OnSceneInit()

    BattleBG  = animation_create(ADDONWDF, 0xE3B87E0F)
    magic_tsv = content_system_get_table('magic')
    MagicAnim = animation_create(6,0x29D819DF)
    MagicAnim:EnableDrag(true)
    MagicAnim:SetLoop(0)
    MagicAnim:SetFrameInterval(0.016*8)
    MagicAnim:SetVisible(false)

    local ostime = os.time()
    enemy  = actor_manager_create_actor(ostime)
    enemy:SetProperty(PROP_AVATAR_ID,'JMW-AXE')
    enemy:SetProperty(PROP_WEAPON_AVATAR_ID,'JMW-AXE-120-护法灭魔')
    enemy:SetProperty(PROP_NAME, '巨魔王')
    enemy:SetProperty(PROP_POS, 206, 190)
    enemy:ResetASM()
    enemy:SetActionID(ACTION_BATIDLE)
    enemy:ChangePalMatrix(get_pal_from_json('{"1":{"to":40,"mat":[105,273,0,512,0,359,459,412,464],"from":0},"2":{"to":60,"mat":[86,96,134,244,301,144,273,14,330],"from":40},"3":{"to":120,"mat":[24,234,340,325,421,483,345,340,330],"from":60},"4":{"to":256,"mat":[255,0,0,0,255,0,0,0,255],"from":120},"segments":[0,40,60,120,256]}'))
    

    ostime= ostime + 1
    enemyBB  = actor_manager_create_actor(ostime)
    enemyBB:SetProperty(PROP_ACTOR_TYPE, ACTOR_TYPE_PET)
    enemyBB:SetProperty(PROP_AVATAR_ID,'古代瑞兽')
    enemyBB:SetProperty(PROP_WEAPON_AVATAR_ID,'')
    enemyBB:SetProperty(PROP_POS, 274, 250)
    enemyBB:ChangePalMatrix(get_pal_from_json('{"1":{"to":256,"mat":[52,0,0,88,254,204,161,204,154],"from":0},"segments":[0,256]}')  )
    enemyBB:SetProperty(PROP_NAME, '古代瑞兽')
    enemyBB:ResetASM()
    enemyBB:SetActionID(ACTION_BATIDLE)

    
    ostime = ostime + 1
    playerBB  = actor_manager_create_actor(ostime)
    playerBB:SetProperty(PROP_ACTOR_TYPE, ACTOR_TYPE_PET)
    playerBB:SetProperty(PROP_AVATAR_ID,'鬼将')
    playerBB:SetProperty(PROP_NAME, '鬼将')
    playerBB:SetProperty(PROP_WEAPON_AVATAR_ID,'')
    playerBB:SetProperty(PROP_POS,563,376)
    playerBB:ResetASM()
    playerBB:SetActionID(ACTION_BATIDLE)
    playerBB:ChangePalMatrix(get_pal_from_json('{"1":{"to":256,"mat":[115,115,110,163,256,239,196,91,292],"from":0},"segments":[0,256]}'))
    
    

    ostime= ostime + 1
    player = actor_manager_create_actor(ostime)
    actor_manager_set_local_player(player:GetID())
    player:SetProperty(PROP_AVATAR_ID,'JXK-SWORD')
    player:SetProperty(PROP_WEAPON_AVATAR_ID,'JXK-SWORD-120-四法青云')
    local new_pal = get_pal_from_json('{"1":{"mat":[220,220,512,512,512,510,512,510,512],"to":40,"from":0},"2":{"mat":[211,211,230,445,292,445,306,306,206],"to":80,"from":40},"3":{"mat":[0,0,0,24,24,24,19,19,19],"to":120,"from":80},"4":{"mat":[255,0,0,0,255,0,0,0,255],"to":256,"from":120},"segments":[0,40,80,120,256]}')  
    player:ChangePalMatrix(new_pal)
    
    player:SetDir(0)
    player:ReverseDir()
    player:SetProperty(PROP_POS, 616,412)
    player:SetProperty(PROP_NAME, '剑侠客')
    player:ResetASM()
    player:SetActionID(ACTION_BATIDLE)


    playerBB:SetDir(player:GetDir())
    enemy:SetDir(player:GetDir())
    enemy:ReverseDir()

    enemyBB:SetDir(enemy:GetDir())
    
    player:SetTarget(enemy)
    enemy:SetTarget(player)

    playerBB:SetTarget(enemyBB)
    enemyBB:SetTarget(playerBB)
end

local actor_dir = 0
local actor_action = 0
local actor_frame = 0
local actor_show_boudingbox={}

function OnSceneImGuiUpdate()
    imgui.SetCursorPos(50,100)
    imgui.BeginGroup()

    -- if imgui.Button('Translate') then
    --     MagicAnim:Pause(200)
    --     MagicAnim:Translate(0,-200,300)
    -- end

    -- if imgui.Button('LockFrame') then
    --     -- MagicAnim:Pause(200)
    --     MagicAnim:LockFrame(1)
    -- end

    imgui.EndGroup()
    -- MagicAnim:DrawBoundingBox()
end

function OnSceneUpdate()
   	actor_manager_update()
    MagicAnim:Update()
    
end

function OnSceneDraw()
    BattleBG:Draw()
    actor_manager_draw()
    MagicAnim:Draw()
end

