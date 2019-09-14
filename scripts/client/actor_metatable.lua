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

function ActorMT:DrawAvatarInfo()
    local sprite = self:GetAvatar()
    draw_avatar_info(sprite)
end

function ActorMT:DrawBoundingBox()
    local avatar = self:GetAvatar()
    draw_avatar_boundingbox(avatar)
end


local AnimationMT = animation_get_metatable()
function AnimationMT:DrawAvatarInfo()
    draw_avatar_info(self)
end


function AnimationMT:DrawBoundingBox()
    draw_avatar_boundingbox(self)
end

local actor_enum_typename_to_type ={
    bool = PROP_TYPE_BOOL,
    int  = PROP_TYPE_INT,
    uint64 = PROP_TYPE_UINT64,
    float = PROP_TYPE_FLOAT,
    str  = PROP_TYPE_STR,
    vec2 = PROP_TYPE_VEC2
}

function on_actor_reg_props(actor)
    local tbl = content_system_get_table('actor_template')
    for i,row in ipairs(tbl) do
        local reg_type = actor_enum_typename_to_type[row.type]
        local v         
        if row.type == 'bool' then
            v = row.def == 'true'
        elseif row.type == 'int' or row.type == 'uint64' then
            v = math.tointeger(row.def)
        elseif row.type == 'float' then
            v = tonumber(row.def)
        elseif row.type == 'str' then
            v = row.def
        elseif row.type == 'vec2' then
            local strx,stry = string.match(row.def,'{(.-),(.-)}')
            v = {}
            table.insert(v, tonumber(strx))  
            table.insert(v,tonumber(stry)) 
        end
        if row.type ~= 'vec2' then
            actor:RegProperty(i-1,reg_type,v)
        else
            actor:RegProperty(i-1,reg_type,v[1],v[2])
        end
    end


    -- for i,row in ipairs(tbl) do
    --     cxlog_info(actor:GetProperty(i-1))
    -- end 
end