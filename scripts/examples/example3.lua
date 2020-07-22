--[[
CXEngine.exe --cwd=[工作目录] --script_path=[lua文件夹相对工作目录]/  --luapath=[相对script_path下的lua文件路径]
示例：
./bin/Debug/CXEngine.exe --cwd=[CXEngine目录的路径]/  --script_path=scripts/examples/ --luapath=example0.lua

example3 创建一个actor

创建一个actor比较复杂，会引入scripts/share里面的一些lua文件
]]

--script_system_dofile 相当于lua里面的dofile，只不过会把传入的是lua文件相对于项目目录的相对路径拼接成磁盘上的绝对路径
script_system_dofile('../share/enums.lua')
script_system_dofile('../share/vfs.lua')
script_system_dofile('../share/utils.lua')
script_system_dofile('../share/content_system.lua')
script_system_dofile('../share/actor_metatable.lua')


local actor
function init()
    content_system_init()           --这个函数在content_system.lua里面定义，会加载各种数据表(tables/*.tsv)，游戏内容基本上是由表来定义的
    actor = lua_create_actor(1)     --lua_create_actor的参数是actor的id，这里就随便填了个1
    actor:ResetASM()                --ResetASM是重新设置actor的动作状态机，目前需要这么手动调用一下

    --actor这个对象很有用，每个actor对象都定义了很多属性，可以通过Get/SetProperty来读写，具体actor的属性定义文件在res\tables\actor_template.tsv中
    actor:SetProperty(PROP_NAME, 'yzxy')    --通过SetProperty来设置玩家的名字
    actor:SetPos(100, 100)                  --设置玩家的世界坐标
end

function update()
    if actor then
        actor:Update()
        actor:Draw()
    end
end

do
    iw_init(800, 600)
    local ok, ret = pcall(init)
    if not ok then
        print(ret)
    end
    iw_render(update)
    iw_deinit()
end
