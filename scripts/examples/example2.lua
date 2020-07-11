local anim
function init()
    anim = animation_create(23,0xCA8FDEAD)  
    anim:SetPos(200,100)
    anim:SetLoop(0)
    anim:Play()
end

function update()
    if anim then
        anim:Update()
        anim:Draw()
    end
end

--程序运行从这里开始
do
    iw_init(800,600)            --创建一个窗口（800x600）
    init()                      --初始化函数
    iw_render(update)           --update函数注册到窗口更新回调
    iw_deinit()                 --程序结束
end