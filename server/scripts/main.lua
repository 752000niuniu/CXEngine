-- local function debugger_loop(server)

-- end
-- coro_debugger = coroutine.create(debugger_loop)
local cjson = require("cjson")

local trap_wait = false
local what = 12123
local xxxx = "asdqwew asd "
function debug_hook(event, line)
    print('debug_hook',event,line)
    while trap_wait do
        lua_run_loop()
        lua_sleep(10)
    end
end

function SetHook()
    print('Run()')
    debug.sethook(debug_hook, "lcr")

end

function SetTrapWait(bWait)
    print(bWait)
    trap_wait= bWait
end

function TestStep3()
    local step1 = 1
    local step2 = 2
    local step3 = 3
    -- fetch_stacks()
    -- fetch_vars()
end

function TestStep2()
    local step1 = 1
    local step2 = 2
    local step3 = 3
    TestStep3()
end

function TestStep()
    local step1 = 1
    local step2 = 2
    local step3 = 3
    TestStep2()
end
local update = 0
function OnGameUpdate()
    print('OnGameUpdate' .. tostring(update))
    update = update + 1
    local c = 3
    local d = 1
    local e = 4

    TestStep()


end

function DoSomething()

end

function tostringex(v)
    if type(v) == 'table' then
        local count = 0
        for _ in pairs(v) do count = count + 1 end
        return string.format('table(%i)', count)
    else return tostring(v) end
end

function fetch_locals(lv)
    print("level:"..tostring(lv))
    local i = 1
    local locals = {}
    while true do
        local k,v = debug.getlocal(lv,i)
        if not k then i = 1 ; break end
        if k:sub(1,1) ~= '(' then table.insert(locals, {type(k),k,tostringex(v),type(v)}) end
        i = i + 1
    end
    while true do
        local k, v = debug.getlocal(lv, -i)
        if not k then break end
        table.insert(locals, {type(k), k, tostringex(v), type(v)})
        i = i + 1
    end
    return cjson.encode(locals)
end



print("hello world")