local anim
function init()
    anim = animation_create(23,0xCA8FDEAD)  
    anim:SetLoop(0)
    anim:Play()
end

function update()
    if anim then
        anim:Update()
        anim:Draw()
    end
end

do
    iw_init(800,600)
    init()
    iw_render(update)
    iw_deinit()
end