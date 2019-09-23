local ActorMT = actor_get_metatable()

function draw_avatar_info(sprite)
    local x,y = sprite:GetPos()
    imgui.Text('X '.. math.floor(x) ..' Y'.. math.floor(y))
    imgui.Text('w '.. sprite:GetWidth()..' h '.. sprite:GetHeight())
    imgui.Text('KeyX '.. sprite:GetKeyX().. ' KeyY '.. sprite:GetKeyY())

    local curframe = sprite:GetCurrentFrame()
    imgui.Text('CurKeyX '.. sprite:GetFrameKeyX(curframe) ..' CurKeyY '.. sprite:GetFrameKeyY(curframe))

    imgui.Text('PlayTime'.. ' '.. string.format('%.2f',sprite:GetPlayTime()) )
    imgui.Text('DirCnt'.. ' '.. sprite:GetDirCnt())
    imgui.Text('TotalFrames'.. ' '.. sprite:GetTotalFrames())
    imgui.Text('CurrentFrame'.. ' '.. sprite:GetCurrentFrame())
    imgui.Text('GroupFrameCount'.. ' '.. sprite:GetGroupFrameCount())
    imgui.Text('GroupCount'.. ' '.. sprite:GetGroupCount())
    imgui.Text('dir '.. sprite:GetDir() .. ' dt '.. string.format('%.2f',sprite:GetFrameInterval()) )
end

function draw_avatar_boundingbox(avatar)
    if not avatar then return end
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

local actor_frame = 1
function ActorMT:DrawAvatarInfo()
    local actor = self
    local avatar = self:GetAvatar()

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

    draw_avatar_info(avatar)
end

function ActorMT:DrawBoundingBox()
    local avatar = self:GetAvatar()
    if not avatar then return end
    draw_avatar_boundingbox(avatar)
end

function ActorMT:SetProperties(props)
    for k,v in pairs(props) do
        if type(v) ~='table' then
            self:SetProperty(k,v)
        else
            self:SetProperty(k, table.unpack(v))
        end
    end
    local avatar = self:GetAvatar()
    draw_avatar_boundingbox(avatar)
end


local AnimationMT = base_sprite_get_metatable()
function AnimationMT:DrawAvatarInfo()
    draw_avatar_info(self)
end


function AnimationMT:DrawBoundingBox()
    draw_avatar_boundingbox(self)
end
