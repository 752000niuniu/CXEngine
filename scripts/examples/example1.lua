script_system_dofile('../share/enums.lua')
script_system_dofile('../share/vfs.lua')
script_system_dofile('../share/utils.lua')
script_system_dofile('../share/content_system.lua')
script_system_dofile('../share/actor_metatable.lua')


local actor
function init()
    content_system_init()
    actor = lua_create_actor(1)
    actor:ResetASM()
    actor:SetProperty(PROP_NAME, 'yzxy')
    actor:SetPos(100, 100)
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
