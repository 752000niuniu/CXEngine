
function utils_string_split(str, cut)
    str = str..cut
    local pattern  = '(.-)'..cut
    local res = {}
    for w in string.gmatch(str, pattern) do
        table.insert(res,w)
        --print(w)
    end
    return res
end



function utils_dump_table(t)
    if not t or type(t)~='table' then return end
   
    local count = 1 
    local next_layer = {}
    table.insert(next_layer, t)
    while true do 
        if #next_layer == 0 then break end
        local next_t = table.remove(next_layer,1)
        for k,v in pairs(next_t) do 
            if type(v) == 'table' then
                if count > 5 then break end
                count = count + 1
                table.insert(next_layer, v)
            else
                print(k,v)
            end
        end    
    end
end 


function on_script_system_init()
    game_server_start()
end

function on_script_system_update()
    game_server_update()
    return true
end


function on_script_system_deinit()
    game_server_stop()
end