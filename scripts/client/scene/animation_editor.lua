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
    BeHitAnim =  base_sprite_create(ADDONWDF,0x1D3FF13C)

    player = actor_manager_create_actor(os.time())
    player:SetRoleID(math.tointeger(RoleIDSB:str()))
    player:SetWeaponID(math.tointeger(WeaponIDSB:str()))
    player:SetActionID(ACTION_ATTACK)
    player:SetDir(2)
    player:SetX(615.0)
    player:SetY(275.0)
    -- player:GetAvatar():Stop()

    enemy  = actor_manager_create_actor(os.time() + 100)
    enemy:SetType(ACTOR_TYPE_PET)
    
    enemy:SetRoleID(35)
    enemy:SetWeaponID(0)
    enemy:SetActionID(ACTION_BEHIT)
    enemy:SetDir(player:GetDir())
    enemy:ReverseDir()
    enemy:SetX(375.0)
    enemy:SetY(170.0)
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

local actor_dir = 0
local actor_action = 0
local actor_frame = 0
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

        if imgui.Button('ActionCapter##'..actor:GetID()) then
            local file = io.open(vfs_makepath('a.txt'), 'a+')
            file:write('ROLE:'..actor:GetRoleID()..' Cur:'..avatar:GetCurrentFrame()..' Total:'..avatar:GetGroupFrameCount()..'\n')
            file:close()
        end

        local curframe = avatar:GetCurrentFrame()
        draw_sprite_info(avatar)
        local  orix , oriy = scene_manager_get_imgui_cursor_pos()
        local avx, avy = avatar:GetPos()
        local tlx = orix + avx 
        local tly = oriy + avy
        local center_x = tlx + avatar:GetKeyX()
        local center_y = tly + avatar:GetKeyY()
        imgui.AddCircleFilled(center_x,center_y,4,0xff0000ff)

        local brx = orix + avx + avatar:GetWidth()
        local bry = oriy + avy + avatar:GetHeight()
        imgui.AddRect(tlx,tly,brx,bry,0xff00ffff)

        center_x = center_x - avatar:GetFrameKeyX(curframe)
        center_y = center_y - avatar:GetFrameKeyY(curframe)
        imgui.AddCircleFilled(center_x,center_y,4,0xff00ff00)

        brx = center_x + avatar:GetFrameWidth(curframe)
        bry = center_y + avatar:GetFrameHeight(curframe)
        imgui.AddRect(center_x,center_y,brx,bry,0xff00ff00)
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
    BeHitAnim:Draw()
    player:Draw()
end

