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
    magic_tsv =   content_system_get_table('magic')
    MagicAnim =  animation_create(6,0x387C1EEB)
    MagicAnim:EnableDrag(true)
    MagicAnim:SetLoop(0)
    MagicAnim:SetFrameInterval(0.016*8)
    -- MagicAnim:SetVisible(false)
    player = actor_manager_create_actor(os.time())
    actor_manager_set_local_player(player:GetID())
    player:SetProperty(PROP_AVATAR_ID,'JXK-SWORD')
    player:SetProperty(PROP_WEAPON_AVATAR_ID,'JXK-SWORD-060-X')
    -- player:SetType(ACTOR_TYPE_PET)
    -- player:SetProperty(PROP_AVATAR_ID,'龙龟')
    player:SetDir(0)
    player:SetProperty(PROP_POS,375,170)
    player:SetActionID(ACTION_IDLE)
    player:ResetASM()

    enemy  = actor_manager_create_actor(os.time() + 1)
    enemy:SetProperty(PROP_AVATAR_ID,'JMW-KNIFE')
    enemy:SetProperty(PROP_WEAPON_AVATAR_ID,'JMW-KNIFE-060-X')
    -- enemy:SetActionID(ACTION_BEHIT)
    enemy:SetDir(player:GetDir())
    enemy:ReverseDir()
    enemy:SetProperty(PROP_POS,615,275)

    enemy:SetActionID(ACTION_IDLE)
    enemy:ResetASM()
end

local actor_dir = 0
local actor_action = 0
local actor_frame = 0
local actor_show_boudingbox={}
function imgui_draw_actor(actor)
    if actor then
        local x ,y  = actor:GetProperty(PROP_POS)
        local avatar = actor:GetAvatar()
        if not avatar then return end
        imgui.SetCursorPos(x-55,y+50)
        imgui.BeginGroup()
        
        if imgui.Button('+##Frame'..actor:GetID(),30) then
            actor_frame = actor_frame+1
            if actor_frame >= avatar:GetGroupFrameCount() then actor_frame  = 0 end
            avatar:LockFrame(actor_frame)    
        end
        imgui.SameLine();imgui.Text('Frame');imgui.SameLine()
        if imgui.Button('-##Frame'..actor:GetID(),30) then
            actor_frame = actor_frame-1
            if actor_frame <= 0 then actor_frame  = avatar:GetGroupFrameCount() end
            avatar:LockFrame(actor_frame)    
        end

        if imgui.Button('Play##'..actor:GetID()) then
            avatar:UnLockFrame()
        end
        imgui.SameLine()
        if imgui.Button('Stop##'..actor:GetID()) then
            avatar:Stop()
        end

        if imgui.Button('ExportWas##'..actor:GetID()) then
            local path = vfs_makepath('a.was')
            avatar:ExportWas(path)
        end

        res, actor_show_boudingbox[actor:GetID()] = imgui.Checkbox('BoudingBox###bb'..actor:GetID(),actor_show_boudingbox[actor:GetID()] or false)
        local curframe = avatar:GetCurrentFrame()
        actor:DrawAvatarInfo()
        if  actor_show_boudingbox[actor:GetID()] then
            actor:DrawBoundingBox()
        end
        
        imgui.EndGroup()
    end
end

function OnSceneImGuiUpdate()
    imgui.SetCursorPos(50,100)
    imgui.BeginGroup()

    if imgui.Button('Translate') then
        MagicAnim:Pause(200)
        MagicAnim:Translate(0,-200,300)
    end

    if imgui.Button('LockFrame') then
        -- MagicAnim:Pause(200)
        MagicAnim:LockFrame(1)
    end


    if imgui.Button('EnemyAttack') then
        enemy:PlayAttack(player)
    end

    if imgui.Button('EnemyCast') then
        enemy:PlayCast(player,0xC12C2663)
    end

    
    if imgui.Button('+TimeInterval') then
        TimeInterval = TimeInterval + 0.016
        player:SetTimeInterval(TimeInterval)
        enemy:SetTimeInterval(TimeInterval)
    end

    if imgui.Button('-TimeInterval') then
        TimeInterval = TimeInterval -0.016
        player:SetTimeInterval(TimeInterval)
        enemy:SetTimeInterval(TimeInterval)
    end
    
    imgui.EndGroup()
    imgui_draw_actor(player)
    imgui_draw_actor(enemy)   
    MagicAnim:DrawBoundingBox()
end

function OnSceneUpdate()
   	actor_manager_update()
    MagicAnim:Update()
    
end

function OnSceneDraw()
    actor_manager_draw()
    MagicAnim:Draw()
    
end

