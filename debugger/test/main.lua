
debugger_start_session(9527)

function  what_the_f1()
    local a = 1
    local b = 2
    local c = a + b
end

function  what_the_f2()
    what_the_f1()
end

do
    local last_time = os.time()
    while true do 
        local now_time = os.time()
        if now_time - last_time >= 1 then
            debugger_update_session_new()
            
            what_the_f2()
        end
        last_time = now_time
    end
    debugger_stop_session()
end