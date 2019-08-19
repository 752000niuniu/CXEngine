local demo_count = 2
local demo_speed = 20
local coffient = 20 * 0.064
local demo_dir = {x=1,y=1}
-- glm::vec2 demo_dir = glm::normalize(glm::vec2(1, 1));
local demo_distance = 600 
local sprites_pair = {}
local sel_sps ={}
local player 
local enemy
local added_frame = 0

--剑侠客 攻击
---待战-> Runto-> Attack-> Runback -> 待战
function OnSceneInit()
    player = actor_manager_create_actor(os.time())
    player:SetRoleID(1)
    player:SetWeaponID(40)
    player:SetActionID(ACTION_IDLE)
    

    player:SetX( 615.0)
    player:SetY(275.0)

    enemy  = actor_manager_create_actor(os.time() + 100)
    enemy:SetType(ACTOR_TYPE_PET)
    enemy:SetRoleID(7)
    enemy:SetWeaponID(0)
    enemy:SetActionID(ACTION_IDLE)
    
    enemy:SetX(375.0)
    enemy:SetY(170.0)
    
end

function OnSceneUpdate()
    player:Update()
    enemy:Update()
    for i=1,#sprites_pair do
        local sprite = sprites_pair[i]
        sprite:SetCurrentFrame(added_frame)        
    end    
end

local added_action = 0

local RoleIDSB = imgui.CreateStrbuf('1',256)
local WeaponIDSB = imgui.CreateStrbuf('60',256)
local LoadActionID = ACTION_IDLE
local TimeInterval = 0.016 * 4
function OnMergeSpriteClick()
    local sp1 =  sprites_pair[1]
    local sp2 =  sprites_pair[2]
    sp1:SetFrameInterval(TimeInterval)
    sp2:SetFrameInterval(TimeInterval)

    local x,y = sp1:GetPos()
    local sp1KeyX,sp1KeyY = sp1:GetKeyX() ,sp1:GetKeyY()
    local sp2KeyX,sp2KeyY = sp2:GetKeyX() ,sp2:GetKeyY()

    local curframe = sp1:GetCurrentFrame()
    local sp1CurFrameKeyX ,sp1CurFrameKeyY = sp1:GetFrameKeyX(curframe) ,sp1:GetFrameKeyY(curframe)
    local sp2CurFrameKeyX ,sp2CurFrameKeyY = sp2:GetFrameKeyX(curframe) ,sp2:GetFrameKeyY(curframe)

    local sp2X  = x+ (sp1KeyX-sp2KeyX)
    local sp2Y  = y+ (sp1KeyY-sp2KeyY)
    sp2:SetPos(sp2X , sp2Y)
end

function OnChangeAction()
    for i,v in ipairs(sprites_pair) do
        v:Destroy()
    end
    sprites_pair = {}
    local roleID = math.tointeger(RoleIDSB:str())
    local weaponID = math.tointeger(WeaponIDSB:str())
    local role_was = game_get_action_was_id(ACTOR_TYPE_PLAYER, roleID, LoadActionID)
    local weapon_was = game_get_weapon_was_id(weaponID, LoadActionID)
    local sprite = base_sprite_create(ShapeWDF, role_was)
    local weapon_sprite = base_sprite_create(ShapeWDF, weapon_was)
    table.insert(sprites_pair,sprite)
    table.insert(sprites_pair,weapon_sprite)

    for i,v in ipairs(sprites_pair) do
        v:SetPos(300,200)
    end
end
function OnChangeTimeInterval()
    for i,v in ipairs(sprites_pair) do
        v:SetFrameInterval(TimeInterval)
    end
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
function imgui_draw_actor(actor)
    if actor then
        local x ,y = actor:GetX() , actor:GetY()
        local h = actor:GetHeight()
        local w = actor:GetWidth()
        local avatar = actor:GetAvatar()
        imgui.SetCursorPos(x-w//2,y+20)
        imgui.BeginGroup()
        if imgui.Button('ChangeDir##'..actor:GetID()) then
            actor_dir  = actor_dir +1
            if actor_dir > 7 then actor_dir = 0 end
            actor:SetDir(math.tointeger(actor_dir))
        end

        if imgui.Button('ChangeAction##'..actor:GetID()) then
            actor_action = actor_action+1
            if actor_action> 15 then actor_action  = 0 end
            actor:SetActionID(math.tointeger(actor_action))
        end
        
        draw_sprite_info(avatar)
        local  orix , oriy = scene_manager_get_imgui_cursor_pos()
        local avx, avy = avatar:GetPos()
        local tlx = orix + avx 
        local tly = oriy + avy
        
        local brx = orix + avx + avatar:GetWidth()
        local bry = oriy + avy + avatar:GetHeight()
        imgui.AddRect(tlx,tly,brx,bry,0xff00ffff)
        imgui.EndGroup()
    end


end
function OnSceneImGuiUpdate()
    imgui.SetCursorPos(0,0)
    imgui.BeginGroup()
    imgui.InputText("RoleID", RoleIDSB)
    imgui.InputText("WeaponID", WeaponIDSB)

    if imgui.Button('Attack') then
        player:PlayAttack(enemy)
    end
    if imgui.Button('LoadPlayer') then
        local roleID = math.tointeger(RoleIDSB:str())
        local weaponID = math.tointeger(WeaponIDSB:str())
        player:SetRoleID(roleID)
        player:SetWeaponID(weaponID)
    end
    if imgui.Button('+ActionID') then
        LoadActionID = LoadActionID + 1
        added_frame = 0
        if LoadActionID > 15 then
            LoadActionID = 0
        end
        OnChangeAction()
        OnMergeSpriteClick()
    end

    if imgui.Button('-ActionID') then
        LoadActionID = LoadActionID- 1
        added_frame = 0
        if LoadActionID <  0 then
            LoadActionID = 15
        end
        OnChangeAction()
        OnMergeSpriteClick()
    end

    if imgui.Button('+TimeInterval') then
        TimeInterval = TimeInterval + 0.016
        player:SetTimeInterval(TimeInterval)
        enemy:SetTimeInterval(TimeInterval)
        -- OnChangeTimeInterval()
    end

    if imgui.Button('-TimeInterval') then
        TimeInterval = TimeInterval -0.016
        player:SetTimeInterval(TimeInterval)
        enemy:SetTimeInterval(TimeInterval)
        -- OnChangeTimeInterval()
    end
    if imgui.Button('+AddFrame') then
        added_frame  = added_frame + 1
        if added_frame >= sprites_pair[1]:GetGroupFrameCount() then
            added_frame = added_frame-1
        end
    end
    if imgui.Button('-AddFrame') then
        added_frame  = added_frame - 1
        if added_frame <0 then
            added_frame  = 0
        end
    end
    
    imgui.Text('LoadActionID:' .. LoadActionID)
    imgui.Text('TimeInterval:' .. TimeInterval)
    if imgui.Button('LoadRole') then
        OnChangeAction()
        OnChangeTimeInterval()
        for i,v in ipairs(sprites_pair) do
            v:SetFrameInterval(TimeInterval)
            v:SetPos(300,200)
        end
        OnMergeSpriteClick()
    end

    if imgui.Button('合并') then
        OnMergeSpriteClick()
    end

    if imgui.Button('改方向') then
        local sp1 =  sprites_pair[1]
        local sp2 =  sprites_pair[2]
        local dir  = sp1:GetDir()+1
        if dir > 7 then dir = 0 end
        sp1:SetDir(math.tointeger(dir))
        sp2:SetDir(math.tointeger(dir))
    end
    
    if imgui.Button('导出图片') then
        local sp1 =  sprites_pair[1]
        local sp2 =  sprites_pair[2]
        sp1:Export('E:/Github/SimpleEngine/res/assets/', 'sp1_')
        sp2:Export('E:/Github/SimpleEngine/res/assets/', 'sp2_')
    end

    if imgui.Button('攻击') then
        
    end

    if imgui.Button('施法') then
        
    end

    imgui.EndGroup()
    imgui_draw_actor(player)
    imgui_draw_actor(enemy)
 

    for i=1,#sprites_pair do
        local sprite = sprites_pair[i]
        local x ,y = sprite:GetPos()
        local h = sprite:GetHeight()        
        imgui.SetCursorPos(x,y+h)
        imgui.BeginGroup()
        draw_sprite_info(sprite)
        imgui.EndGroup()
    end    

    
end

function OnSceneDraw()
    player:Draw()
    enemy:Draw()
    for i=1,#sprites_pair do
        sprites_pair[i]:Draw()
    end    
end

