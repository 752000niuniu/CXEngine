local ActorMT = actor_get_metatable()


function ActorMT:DrawAvatarInfo()
    local sprite = self:GetAvatar()
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



function ActorMT:DrawBoundingBox()
    local avatar = self:GetAvatar()
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


