

function on_scene_manager_init()
    -- &#x85CF;&#x5FC3
    -- local s  = "Ocean\u{85CF}\u{5FC3}" --  ..string.char(0x85CF,0xCF,0x5F,0xC3)


    -- local x = 3190
    -- local y = 1510
    -- local dx = 70
    -- local dy = -60

    -- local sayings =
    -- {
    --     "巴西队必胜！",
    --     "德国队必胜！",
    --     "意大利队必胜！",
    --     "阿根廷队必胜！",
    --     "西班牙队必胜！",
    --     "荷兰队必胜！",
    --     "哥伦比亚队必胜！",
    --     "墨西哥队必胜！",
    --     "日本队必胜！",
    --     "中国队必胜！"
    -- }

    -- for i=1,10,1 do
    --     scene_add_npc(19,0x49386FCE , s.. tostring(i), x,y,0,3,15, sayings[i])
    --     x = x + dx
    --     y = y + dy
        
    -- end

    -- scene_set_player("oceancx",10,10,3,15)
    -- 3200 1692 
    -- 66 67

    
    script_system_dofile('scene/test_net_scene.lua')
    
end

local t123 = {asd='qwe', sadsa='qweqwe'}
local t2343 = {asd='qwe', sadsa='qweqwe', tasd= {asd = 'asdasd', asdqweqwe='asdlkjjasljd'}}

local function test()
    local a1  = 11
    local b1  = 21
    local c2 = 'sasd '
    local t2 = {asd='qwe', sadsa='qweqwe'}
    local t3 = {asd='qwe', sadsa='qweqwe'}
    local t4 = {asd='qwe', sadsa='qweqwe'}
    local t2343123 = {asd='qwe', sadsa='qweqwe', tasd= {asd = 'asdasd', asdqweqwe='asdlkjjasljd'}}
end
local wthat = '1'
local xx = 'qwelk jqwlke '
local first_gen = true
function on_scene_manager_update()
    local a  = 1
    local b  = 2
    local c = 's '
    local sum = a + b
    local t = {asd='qwe', sadsa='qweqwe'}
    test()
    if first_gen then
        
    end

end

function on_scene_manager_draw()
    
end
