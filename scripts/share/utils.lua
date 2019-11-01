
function utils_fetch_sort_keys(tbl)
    local kset = {}
    for k,v in pairs(tbl) do
        table.insert(kset,k)
    end
    table.sort(kset)
    return kset
end

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


function utils_string_split_fixcnt(str, cut, fixcnt)
    local strs = utils_str_split(str,cut)
    while #strs > fixcnt do
        table.remove(strs,#strs)
    end
    while #strs < fixcnt do
        table.insert(strs,'')
    end
    assert(#strs==fixcnt,'fixcnt is error')
    return strs 
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


function custom_gen_next_sortk_fn(tbl,keys)
    return function(t,st)
        st = st==nil and 1 or (st+1)
        if st>#keys then return nil end
        return st,t[keys[st]],keys[st]
    end
end


function gen_next_sortk_fn(tbl)
    local keys = utils_fetch_sort_keys(tbl)
    return function(t,st)
        st = st==nil and 1 or (st+1)
        if st>#keys then return nil end
        return st,t[keys[st]],keys[st]
    end
end


imgui.HorizontalLayout = function(tbl, next_fn, cb)
    local line_width = imgui.GetContentRegionAvailWidth()
    local cx, cy = imgui.GetCursorPos()
    local layout_x = cx
    do
        local st,v,k
        while true do
            st, v, k = next_fn(tbl, st)
            if st == nil then break end
            if k == nil then k = st end
            cb(k,v)
            local iw,ih = imgui.GetItemRectSize()
            layout_x = layout_x + iw + 8
            if layout_x < line_width-iw-8 then
                imgui.SameLine()
            else
                layout_x = cx 
            end
        end
        if layout_x~= cx then
            imgui.NewLine()
        end
    end
end

function imgui_std_horizontal_button_layout(tbl, next_fn, on_click)
    imgui.HorizontalLayout(tbl,next_fn,function(k,v) 
        if imgui.Button(k) then
            on_click(k,v)
        end
    end)
end

function utils_save_rows_to_tsv(path, col_names, rows, def_row)
    local f = io.open(path,'w+')
    for i, col_name in ipairs(col_names) do
        f:write(col_name..'\t')
    end
    f:write('\n')

    if def_row then
        f:write('*')
        for i,val in ipairs(def_row) do
            f:write(val..'\t')
        end
        f:write('\n')
    end
    
    for i,row in ipairs(rows) do
        for j,name in ipairs(col_names) do
            f:write(row[name]..'\t')
        end
        f:write('\n')
    end
    f:close()
end

function utils_parse_tsv_to_rows(path)
    local tbl = {}
    local col_names = {}
    local index = 1
    for line in io.lines(path) do  
        if index == 1 then
            for col, name in ipairs(utils_string_split(line,'\t')) do
                if name~='' then
                    table.insert(col_names, name)
                end
            end
            -- cxlog_info('colname :'..cjson.encode(col_names))
        else
            if line~='' and not line:match('^%*') then
                local row = {}
                local vals = utils_string_split_fixcnt(line,'\t',#col_names)
                for i,col in ipairs(col_names) do
                    row[col] = vals[i]
                end
                table.insert(tbl,row)
            end
        end
        index = index + 1
    end
    return tbl, col_names
end


function utils_parse_tsv(path, columns)
    local tbl = {}
    local col_names = {}
    local index = 1
    for line in io.lines(path) do  
        if index == 1 then
            for col, name in ipairs(utils_string_split(line,'\t')) do
                if name~='' then
                    table.insert(col_names, name)
                end
            end
            -- cxlog_info('colname :'..cjson.encode(col_names))
        else
            if line~='' and not line:match('^%*') then
                local row = {} 
                local vals = utils_string_split_fixcnt(line,'\t',#col_names)
                for i,key in ipairs(col_names) do
                    local col = columns[i]
                    if col.name == key then
                        if col.def then
                            if vals[i] == '' then
                                vals[i] = col.def
                            end
                        end
                        
                        if col.fmt == 'n' then
                            row[key] = tonumber(vals[i])
                        elseif col.fmt == 'i' then
                            row[key] = math.tointeger(vals[i])
                        elseif type(col.fmt)=='function' then
                            row[key] = col.fmt(vals[i])
                        else
                            row[key] = vals[i]
                        end
                    end
                end
                table.insert(tbl,row)
            end
        end
        index = index + 1
    end
    return tbl, col_names
end

function file_read_int4(file)
    local n = file:read(4)
    n = string.unpack('i4',n)
    return n
end

function decode_mypal(path)
    cxlog_info('decode..'..path)
    local file = io.open(path, 'rb')
    local flag = file:read(4)
    flag = string.unpack('c4',flag)
    assert(flag=='wpal', 'decode_mypal error '..path)

    local seg_num = file_read_int4(file)
    local color_segments = {}
    for i=1,seg_num+1 do
        local sep = file_read_int4(file) 
        table.insert(color_segments,sep)
    end
    -- cxlog_info(color_segments, cjson.encode(color_segments))

    local color_schemes = {}
    color_schemes.segments = color_segments
    for i=1,seg_num do
        local cnt = file_read_int4(file)
        local tbl = {}
        for index = 1,cnt do
            local mat = {}
            for row=1,3 do
                for col=1,3 do
                    local v = file_read_int4(file)
                    table.insert(mat,v)
                end
            end
            table.insert(tbl, mat)
        end
        table.insert(color_schemes,tbl)
    end
    file:close()

    return color_schemes
end

function get_pal_from_json(str)
    local pal = cjson.decode(str)
    local new_pal = {}
    for k,v in pairs(pal) do
        if k~='segments' then
            table.insert(new_pal,v)
        else
            new_pal.segments = v
        end
    end
    return new_pal
end