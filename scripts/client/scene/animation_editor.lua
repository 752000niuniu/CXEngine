local demo_count = 2
local demo_speed = 20
local coffient = 20 * 0.064
local demo_dir = {x=1,y=1}
-- glm::vec2 demo_dir = glm::normalize(glm::vec2(1, 1));
local demo_distance = 600 
local spriteExamples = {}
local sel_sps ={}

local player 
function OnSceneInit()
    player = actor_manager_create_player(os.time())
    player:SetRoleID(9)
    player:SetWeaponID(40)
    player:SetX(200)
    player:SetY(200)
    for i=1,demo_count do 
        local row = (i-1) // 6
        local col = (i-1) % 6
        local playerFrameWasID
        local sprite 
        if i == demo_count then
            playerFrameWasID = game_get_weapon_was_id(40, ACTION_CAST)
            sprite = base_sprite_create(ShapeWDF, playerFrameWasID)
            
        else
            playerFrameWasID = game_get_action_was_id(ACTOR_TYPE_PLAYER, 1, ACTION_CAST)
            sprite = base_sprite_create(ShapeWDF, playerFrameWasID)
        end

        local x = col * sprite:GetWidth() + 20 * col
        local y = row * sprite:GetHeight() + 20* row
        if i== demo_count then
            sprite:SetPos(300, y )
        else
            sprite:SetPos(x+100, y+100 )
        end
        sprite:SetDir(row)
        -- sprite:SetFrameInterval(0.016*(col+2))
        sprite:SetFrameInterval(0.016*4)
        table.insert(spriteExamples, sprite)
        
        table.insert(sel_sps,sprite)    
        cxlog_info('x'..x, 'y'..y)
    end

    
end

function OnSceneUpdate()
    player:Update()
    for i=1,demo_count do
        local sprite = spriteExamples[i]
        sprite:Update()        
    end    
end

local added_action = 0
function OnSceneImGuiUpdate()
    local x ,y = player:GetX() , player:GetY()
    local h = player:GetHeight()
    local w = player:GetWidth()
    imgui.SetCursorPos(x-w//2,y+20)
    imgui.BeginGroup()

    if imgui.Button('ChangeDir') then
        local dir  = player:GetDir()+1
        if dir > 7 then dir = 0 end
        player:SetDir(math.tointeger(dir))
    end

    if imgui.Button('ChangeAction') then
        local action = player:GetActionID()+1
        if action  > 15 then action  = 0 end
        player:SetActionID(math.tointeger(action))
    end

    imgui.EndGroup()

    for i=1,demo_count do
        local sprite = spriteExamples[i]
        local x ,y = sprite:GetPos()
        local h = sprite:GetHeight()
        
        imgui.SetCursorPos(x,y+h)
        imgui.BeginGroup()
        if imgui.Button('Framei'..i) then
            table.insert(sel_sps,sprite)
            if #sel_sps > 2 then
                table.remove(sel_sps,1)
            end
        end
        imgui.Text(string.format('X '.. x))
        imgui.Text(string.format('Y '.. y))
        imgui.Text(string.format('w '.. sprite:GetWidth()))
        imgui.Text(string.format('h '.. sprite:GetHeight()))
        imgui.Text(string.format('KeyX '.. sprite:GetKeyX()))
        imgui.Text(string.format('KeyY '.. sprite:GetKeyY()))
        imgui.Text('PlayTime'.. ' '.. string.format('%.2f',sprite:GetPlayTime()) )
        imgui.Text('DirCnt'.. ' '.. sprite:GetDirCnt())
        imgui.Text('TotalFrames'.. ' '.. sprite:GetTotalFrames())
        imgui.Text('CurrentFrame'.. ' '.. sprite:GetCurrentFrame())
        imgui.Text('GroupFrameCount'.. ' '.. sprite:GetGroupFrameCount())
        imgui.Text('GroupCount'.. ' '.. sprite:GetGroupCount())
        imgui.Text(string.format('dir '.. sprite:GetDir()))
        imgui.Text(string.format('dt '.. string.format('%.2f',sprite:GetFrameInterval()) ))
        
        imgui.EndGroup()
    end    

    imgui.SetCursorPos(50,200)
    if imgui.Button('合并') then
        if not #sel_sps == 2 then return end
        local sp1 =  sel_sps[1]
        local sp2 =  sel_sps[2]
        local x,y = sp1:GetPos()
        local keyx,keyy = sp1:GetKeyX() ,sp1:GetKeyY()
        local key2x,key2y = sp2:GetKeyX() ,sp2:GetKeyY()

        sp1:SetFrameInterval(0.016 *8)
        sp2:SetFrameInterval(0.016 * 8)

        sel_sps[2]:SetPos(x + keyx-key2x,y + keyy-key2y  )
    end

    if imgui.Button('改方向') then
        if not #sel_sps == 2 then return end
        local dir  = sel_sps[1]:GetDir()+1
        if dir > 7 then dir = 0 end

        sel_sps[1]:SetDir(math.tointeger(dir))
        sel_sps[2]:SetDir(math.tointeger(dir))
    end


    if imgui.Button('改动作') then
        if not #sel_sps == 2 then return end
        local sp1 =  sel_sps[1]
        local sp2 =  sel_sps[2]
        for i,v in ipairs(spriteExamples) do
            if v == sp1  then
                v:Destroy()
                table.remove(spriteExamples,i)
                break
            end
        end

        for i,v in ipairs(spriteExamples) do
            if v == sp2  then
                v:Destroy()
                table.remove(spriteExamples,i)
                break
            end
        end

        local action = ACTION_IDLE + added_action
        added_action = added_action + 1
        if added_action > 15 then
            added_action = 0
        end
        playerFrameWasID2 = game_get_action_was_id(ACTOR_TYPE_PLAYER, 1, action)
        sp2 = base_sprite_create(ShapeWDF, playerFrameWasID2)
        sel_sps[2]= sp2
        table.insert(spriteExamples,sp2)

        playerFrameWasID1 = game_get_weapon_was_id(40, action) 
        sp1 = base_sprite_create(ShapeWDF, playerFrameWasID1)
        sel_sps[1]= sp1
        table.insert(spriteExamples,sp1)
    end
    
    if imgui.Button('导出图片') then
        if not #sel_sps == 2 then return end
        local sp1 =  sel_sps[1]
        local sp2 =  sel_sps[2]
        sp1:Export('E:/Github/SimpleEngine/res/assets/', 'sp1_')
        sp2:Export('E:/Github/SimpleEngine/res/assets/', 'sp2_')
    end
end

function OnSceneDraw()
    player:Draw()
    for i=1,demo_count do
        spriteExamples[i]:Draw()
    end    
end
