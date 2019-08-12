local demo_count = 1* 5 + 1
local demo_speed = 20
local coffient = 20 * 0.064
local demo_dir = {x=1,y=1}
-- glm::vec2 demo_dir = glm::normalize(glm::vec2(1, 1));
local demo_distance = 600 
local spriteExamples = {}
function OnSceneInit()
    for i=1,demo_count do 
        local row = (i-1) // 6
        local col = (i-1) % 6
        local playerFrameWasID
        local sprite 
        if i == 6 then
            playerFrameWasID = game_get_weapon_was_id(40, ACTION_WALK)
            sprite = base_sprite_create(ShapeWDF, playerFrameWasID)
            
        else
            playerFrameWasID = game_get_action_was_id(ACTOR_TYPE_PLAYER, 9, ACTION_WALK)
            sprite = base_sprite_create(ShapeWDF, playerFrameWasID)
          
        end

        local x = col * sprite:GetWidth() + 20 * col
        local y = row * sprite:GetHeight() + 20* row
        if i== 6 then
            sprite:SetPos(620, y )
        else
            sprite:SetPos(x, y )
        end
        sprite:SetDir(row)
        sprite:SetFrameInterval(0.016*(col+2))
        table.insert(spriteExamples, sprite)
        
        cxlog_info('x'..x, 'y'..y)
    end



end

function OnSceneUpdate()
    for i=1,demo_count do
        local sprite = spriteExamples[i]
        sprite:Update()        
    end    
end

function OnSceneImGuiUpdate()
    for i=1,demo_count do
        local sprite = spriteExamples[i]
        local x ,y = sprite:GetPos()
        local h = sprite:GetHeight()
        
        imgui.SetCursorPos(x,y+h)
        imgui.BeginGroup()
        imgui.Button('Framei'..i)
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
end

function OnSceneDraw()
    for i=1,demo_count do
        spriteExamples[i]:Draw()
    end    
end