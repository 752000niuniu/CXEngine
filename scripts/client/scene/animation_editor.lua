local player 
local enemy
local added_frame = 0
local added_action = 0
local RoleIDSB = imgui.CreateStrbuf('0',256)
local WeaponIDSB = imgui.CreateStrbuf('3',256)
local LoadActionID = ACTION_IDLE
local TimeInterval = 0.016 * 4

local BeHitAnim 


--剑侠客 攻击
---待战-> Runto-> Attack-> Runback -> 待战
function OnAttackActionCapter(actor, avatar)
    local file = io.open(vfs_makepath('a.txt'), 'a+')
    file:write('ROLE:'..actor:GetRoleID()..' Cur:'..avatar:GetCurrentFrame()..' '..avatar:GetGroupFrameCount()..'\n')
    file:close()
end


function OnSceneInit()
    BeHitAnim =  animation_create(ADDONWDF,0x1D3FF13C)
    BeHitAnim:EnableDrag(true)

    player = actor_manager_create_actor(os.time())
    -- player:SetRoleID(math.tointeger(RoleIDSB:str()))
    -- player:SetWeaponID(math.tointeger(WeaponIDSB:str()))
    player:SetAvatarID('XYS-SWORD')
    player:SetWeaponAvatarID('XYS-SWORD-120-灵犀神剑')
    -- player:SetType(ACTOR_TYPE_PET)
    -- player:SetAvatarID('古代瑞兽')


    player:SetDir(0)
    player:SetX(375.0)
    player:SetY(170.0)

     

    player:SetActionID(ACTION_IDLE)
    -- player:GetAvatar():Stop()

    enemy  = actor_manager_create_actor(os.time() + 1)
    -- enemy:SetType(ACTOR_TYPE_PET)
    -- enemy:SetAvatarID('踏云兽')
    -- enemy:SetDir(0)
    enemy:SetAvatarID('JXK-KNIFE')
    enemy:SetWeaponAvatarID('JXK-KNIFE-120-晓风残月')
    -- enemy:SetRoleID(35)
    -- enemy:SetWeaponID(0)
    -- enemy:SetActionID(ACTION_BEHIT)
    enemy:SetDir(player:GetDir())
    enemy:ReverseDir()
    -- enemy:SetX(375.0)
    -- enemy:SetY(170.0)
    enemy:SetX(615.0)
    enemy:SetY(275.0)
    enemy:SetActionID(ACTION_IDLE)
    -- enemy:GetAvatar():Stop()
end

function draw_sprite_info(sprite)
    local x,y = sprite:GetPos()
    imgui.Text(string.format('X '.. x))
    imgui.Text(string.format('Y '.. y))
    imgui.Text(string.format('w '.. sprite:GetWidth()))
    imgui.Text(string.format('h '.. sprite:GetHeight()))
    imgui.Text(string.format('KeyX '.. sprite:GetKeyX()))
    imgui.Text(string.format('KeyY '.. sprite:GetKeyY()))

    local curframe = sprite:GetCurrentFrame()
    imgui.Text(string.format('CurKeyX '.. sprite:GetFrameKeyX(curframe)))
    imgui.Text(string.format('CurKeyY '.. sprite:GetFrameKeyY(curframe)))

    imgui.Text('PlayTime'.. ' '.. string.format('%.2f',sprite:GetPlayTime()) )
    imgui.Text('DirCnt'.. ' '.. sprite:GetDirCnt())
    imgui.Text('TotalFrames'.. ' '.. sprite:GetTotalFrames())
    imgui.Text('CurrentFrame'.. ' '.. sprite:GetCurrentFrame())
    imgui.Text('GroupFrameCount'.. ' '.. sprite:GetGroupFrameCount())
    imgui.Text('GroupCount'.. ' '.. sprite:GetGroupCount())
    imgui.Text(string.format('dir '.. sprite:GetDir()))
    imgui.Text(string.format('dt '.. string.format('%.2f',sprite:GetFrameInterval()) ))
end

function draw_sprite_bounding_box(avatar)
    local orix , oriy = scene_manager_get_imgui_cursor_pos()
    local avx, avy = avatar:GetPos()
    avx = orix + avx
    avy = oriy + avy
    imgui.AddCircleFilled(avx,avy,2,0xff0000ff)

    local tlx = avx - avatar:GetKeyX()
    local tly = avy - avatar:GetKeyY()
    local brx = tlx + avatar:GetWidth()
    local bry = tly + avatar:GetHeight()
    imgui.AddRect( tlx,tly,brx,bry,0xff00ffff)

    local cx = avx - avatar:GetFrameKeyX(curframe)
    local cy = avy - avatar:GetFrameKeyY(curframe)
    imgui.AddCircleFilled(cx,cy,2,0xff00ff00)

    brx = cx + avatar:GetFrameWidth(curframe)
    bry = cy + avatar:GetFrameHeight(curframe)
    imgui.AddRect(cx,cy,brx,bry,0xff00ff00)
end


local actor_dir = 0
local actor_action = 0
local actor_frame = 0
local actor_show_boudingbox={}
function imgui_draw_actor(actor)
    if actor then
        local x ,y = actor:GetX() , actor:GetY()
        local h = actor:GetHeight()
        local w = actor:GetWidth()
        local avatar = actor:GetAvatar()
        imgui.SetCursorPos(x-w//2,y+20)
        imgui.BeginGroup()
        
        if imgui.Button('+##Dir'..actor:GetID(),30) then
            actor_dir  = actor_dir +1
            if actor_dir > 7 then actor_dir = 0 end
            actor:SetDir(math.tointeger(actor_dir))
        end
        imgui.SameLine();imgui.Text('Dir');imgui.SameLine()
        if imgui.Button('-##Dir'..actor:GetID(),30) then
            actor_dir  = actor_dir - 1
            if actor_dir < 0 then actor_dir = 7 end
            actor:SetDir(math.tointeger(actor_dir))
        end

        if imgui.Button('+##Action'..actor:GetID(),30) then
            actor_action = actor_action+1
            if actor_action> 15 then actor_action  = 0 end
            actor:SetActionID(math.tointeger(actor_action))
        end
        imgui.SameLine();imgui.Text('Action');imgui.SameLine()
        if imgui.Button('-##Action'..actor:GetID(),30) then
            actor_action = actor_action-1
            if actor_action < 0 then actor_action  = 15 end
            actor:SetActionID(math.tointeger(actor_action))
        end

        if imgui.Button('+##Frame'..actor:GetID(),30) then
            actor_frame = actor_frame+1
            if actor_frame >= avatar:GetGroupFrameCount() then actor_frame  = 0 end
            avatar:Stop()
            avatar:SetCurrentFrame(actor_frame)    
        end
        imgui.SameLine();imgui.Text('Frame');imgui.SameLine()
        if imgui.Button('-##Frame'..actor:GetID(),30) then
            actor_frame = actor_frame-1
            if actor_frame <= 0 then actor_frame  = avatar:GetGroupFrameCount() end
            avatar:Stop()
            avatar:SetCurrentFrame(actor_frame)    
        end

        if imgui.Button('Play##'..actor:GetID()) then
            avatar:Play()
        end
        imgui.SameLine()
        if imgui.Button('Stop##'..actor:GetID()) then
            avatar:Stop()
        end
        res, actor_show_boudingbox[actor:GetID()] = imgui.Checkbox('BoudingBox###bb'..actor:GetID(),actor_show_boudingbox[actor:GetID()] or false)

        local curframe = avatar:GetCurrentFrame()
        draw_sprite_info(avatar)
        if  actor_show_boudingbox[actor:GetID()] then
            draw_sprite_bounding_box(avatar)
        end
        
        
        imgui.EndGroup()
    end
end

function OnSceneImGuiUpdate()
    imgui.SetCursorPos(50,100)
    imgui.BeginGroup()

    if imgui.Button('+##roleID', 24) then
        local id = math.tointeger(RoleIDSB:str()) 
        id = id + 1
        RoleIDSB:reset(tostring(id))
    end
    imgui.SameLine()
    if imgui.Button('-##roleID', 24) then
        local id = math.tointeger(RoleIDSB:str()) 
        id = id - 1
        RoleIDSB:reset(tostring(id))
    end
    imgui.PushItemWidth(200)
    imgui.InputText("RoleID", RoleIDSB)
    imgui.PopItemWidth()


    if imgui.Button('+##weaponID', 24) then
        local id = math.tointeger(WeaponIDSB:str()) 
        id = id + 9
        WeaponIDSB:reset(tostring(id))
    end
    imgui.SameLine()
    if imgui.Button('-##weaponID', 24) then
        local id = math.tointeger(WeaponIDSB:str()) 
        id = id - 9
        WeaponIDSB:reset(tostring(id))
    end
    imgui.PushItemWidth(200)
    imgui.InputText("WeaponID", WeaponIDSB)
    imgui.PopItemWidth()

    if imgui.Button('Attack') then
        player:PlayAttack(enemy)
    end


--     3	残月	090643F5
-- 4	失心符	091ADDC7
-- 5	落岩	0AC30DE6
-- 6	未知	0F689A42
-- 7	烟花	11C66BC9
-- 8	泰山压顶	133F8E31
-- 9	楚楚可怜	182CE63B
-- 10	力劈华山	FF33B0EE
-- 11	三味真火	FCDCCCEC
-- 12	奔雷咒	F54C4025
-- 13	姐妹同心	EF3E3FE4
-- 14	红袖添香	E5F52524
-- 15	百毒不侵	D69CAE82
-- 16	龙卷雨击4	D61E2519
-- 17	失忆符	D4AA2F8A
-- 18	追魂符	D4024676
-- 19	千里神行	C813882A
-- 20	佛法无边	C8011EF1
-- 21	唧唧歪歪	C12C2663
-- 22	地狱烈火	BE325D99
-- 23	同甘共苦	B0E5AE1E
-- 24	心如明镜	A7469D8F
-- 25	宁心	A72FDB18
-- 26	日月乾坤	A70D1263
-- 27	双龙戏珠	A680A821
-- 28	神龙摆尾	A4790CE1

    if imgui.Button('Cast1') then
        player:PlayCast(enemy,0x133F8E31)
    end
    if imgui.Button('Cast2') then
        player:PlayCast(enemy,0x783F9D20)
    end
    if imgui.Button('Cast3') then
        player:PlayCast(enemy,0x99ED4688)
    end


    if imgui.Button('EnemyAttack') then
        enemy:PlayAttack(player)
    end

    if imgui.Button('EnemyCast') then
        enemy:PlayCast(player,0xC12C2663)
    end

    if imgui.Button('LoadPlayer') then
        local roleID = math.tointeger(RoleIDSB:str())
        local weaponID = math.tointeger(WeaponIDSB:str())
        player:SetRoleID(roleID)
        player:SetWeaponID(weaponID)
        -- collectgarbage()
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
    -- draw_sprite_info(BeHitAnim)
end

function OnSceneUpdate()
    player:Update()
    enemy:Update()
    local x,y = enemy:GetPos()
    -- BeHitAnim:SetPos(x,y)
    BeHitAnim:Update()
end

function OnSceneDraw()
    enemy:Draw()
    -- BeHitAnim:Draw()
    player:Draw()
end

