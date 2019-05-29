

-- print(content)
function remove_empty_lines(content) 
    local t = {}
    local line_count = 1
    for line in content:gmatch('(.-)\n') do
        -- print(line_count, line)
        line_count = line_count + 1
        if line:match('[^%s]') then
            table.insert(t,line)
        end
    end
    local new_content = table.concat(t,'\n')
    -- print(table.concat(t,'\n'))
    local output_file = io.open([[F:\Github\SimpleEngine\scripts\client\a.txt]],'w')
    output_file:write(new_content)
    output_file:close()
    return  new_content
end

function find_all_macro_if_endif(content)
    for block in content:gmatch('(#if.-#endif)') do
        print(block)
        print('\n')
    end
end

function find_all_type_typedef(content)
    for type, tpdef in content:gmatch('typedef%s*(.-)%s*([%w_]-);') do
        print(type, tpdef)
        for t in type:gmatch('%s*([%w_*]+)%s*') do
            print('\t', t)
        end
    end
    for block in content:gmatch('namespace%s*ImGui%s*(%b{})') do
        for line in block:gmatch('[^{}](.-)\n') do
            if not line:match('^%s*$') then
                print(line)
            end
        end
    end
    for block in content:gmatch('namespace%s*ImGui%s*(%b{})') do
        for ret, fname, args in block:gmatch('IMGUI_API%s*([%w_*&]+)%s*([%w]+)(%b())') do
            print( ret , fname , args)
        end

        for ret, fname, args in block:gmatch('IMGUI_API%s*([%w_*&]+)%s*([%w]+)(%b())') do
            print( ret , fname , args)
        end
    end
end

local buf={}

BufferMT = {}
function BufferMT:ReadableSize()

end

function BufferMT:WriteableSize()

end

function BufferMT:Append()

end

function BufferMT:Preview()

end
setmetatable(buf, BufferMT)

-- find_all_macro_if_endif(content)


imgui_header_separate_flags = {
    { '',                                   'skip'},
    { [[struct ImDrawChannel;]],            'parse struct'},
    { [[typedef unsigned int ImGuiID;]],    'parse typedef'},
    { [[struct ImVec2]],                    'parse ImVec2'},
    { [[struct ImVec4]],                    'parse ImVec4'},
    { [[namespace ImGui]],                  'parse ImGuiAPI'},
    { [[enum ImGuiWindowFlags_]],           'parse enum blocks'},
    { [[#define IMGUI_PAYLOAD_TYPE_COLOR_3F     "_COL3F"]],     'skip'},
    { [[enum ImGuiDataType_]],              'parse enum blocks'},
    { [[struct ImGuiStyle]],                'skip'},
    { [[#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS]],     'skip'},
    { [[#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS]],     'skip'},
    { [[namespace ImGui]],                  'skip'},
    { [[#endif]],                           'skip'},
    { [[enum ImDrawCornerFlags_]],          'parse enum blocks'},
    { [[struct ImDrawList]],                'skip'},
    { [[enum ImFontAtlasFlags_]],           'parse enum blocks'},
    { [[struct ImFontAtlas]],               'skip'},
    { [[enum ImGuiViewportFlags_]],         'parse enum blocks'},
    { [[struct ImGuiViewport]],             'skip'},
    { [[#endif]] ,                          ''}
}

local imgui_enums = {}
local imgui_apis = {}
local imgui_types = {}
local imgui_functions = {}
local imgui_structs = {}

function parse_struct(content)
    for line in content:gmatch('(.-)\n') do
        if line:find('struct') then
            local last_word 
            for word in line:gmatch('([^%s;]+)') do
                last_word = word
            end
            if last_word then
                table.insert(imgui_structs, last_word)
            end
        end
    end
    -- for i,struct_str in ipairs(imgui_structs) do
        -- print(i, struct_str)
    -- end
end
----------------------------------------
-- function_prototype 函数原型
-- [const][rettype][*|&] func([const][type][*|&][name][[10]])
-- type = {
--     is_const,
--     is_ptr,
--     is_ref,
--     is_array,
--     array_n,
--     ele_type,
-- }
----------------------------------------
-- PlotLines(const char* label,
--  const float* values,
--  int values_count,
--  int values_offset = 0,
--  const char* overlay_text = NULL,
--  float scale_min = FLT_MAX,
--  float scale_max = FLT_MAX,
--  ImVec2 graph_size = ImVec2(0, 0),
--  int stride = sizeof(float));

int lua_plot_lines(luaState* L){
    local arg1 = luacheckstring(label)

    return 0 
}
local imgui_unsupported_types = 
{
    ImGuiContext = true,
    ImGuiInputTextCallback  = true,
    ImGuiStorage = true,
    ImGuiViewport = true,
    ImFontAtlas = true,
    ImGuiDockFamily = true,
    ImGuiStyle = true,
    ImFont = true,
    ImGuiSizeCallback = true,
    ['...'] = true, 
    va_list = true
}

local imgui_api_arg_types = {}

function parse_funcargs_cap(args)
    if args == '()' then return end
    args = args:sub(2,#args-1)

    local brace_repls ={}
    args = args:gsub('(%b())',function(cap)
        table.insert(brace_repls, cap)
        return '@'..#brace_repls
    end)

    args = args..','
    for arg_block in args:gmatch('(.-),') do
        arg_block = arg_block:gsub('@', ' @', 1)
        print('\t', arg_block)
        if arg_block:find('%.%.%.') then
            imgui_api_arg_types['...'] = true
        else
            local equal_left
            local equal_right
            local l,r = arg_block:find('=') 
            if l then
                equal_left = arg_block:sub(1,l-1)
                equal_right = arg_block:sub(r+1)
            else 
                equal_left = arg_block
            end

            local atype 
            local aname 
            local words = {}
            for w in equal_left:gmatch('([@%w_*&%[%]]+)') do
                table.insert(words, w)
            end
            aname = words[#words]
            table.remove(words,#words)
            atype = table.concat(words,' ')
            imgui_api_arg_types[atype] = true
        end
    end

    
    return args
end

function parse_typedef(content)
    for line in content:gmatch('(.-)\n') do
        -- print('parse_typedef', line )
        if line:find('typedef') and line:find('ImS64')==nil and line:find('ImU64')==nil then
            if line:find('typedef.+%b()%s*%b();') then
                local ret_type, ret_dec, fname_cap, args_cap = line:gmatch('typedef%s*([%w_]+)%s*([&*]?)%s*(%b())(%b());')()
                local fname = fname_cap:gmatch('[*]%s*([%w_]+)')()
                -- local args = parse_funcargs_cap(args_cap)
                -- print('find function', ret_type, ret_dec, fname, args)             
                imgui_types[fname] =  ret_type..ret_dec..' (*)'..args_cap
            else
                local words = {} 
                for word in line:gmatch('([^%s;]+)') do
                    table.insert(words, word)
                end
                if #words > 2 then
                    local first = words[1]
                    local last = words[#words]
                    table.remove(words,1)
                    table.remove(words,#words)
                    imgui_types[last] = table.concat(words,' ')    
                end
            end
        end
    end
    local i = 1
    for k,w in pairs(imgui_types) do
        -- print('imgui_types ',i, k,'=', w)
        i = i + 1
    end
end

function parse_imvec2(content)
    imgui_types['ImVec2'] = 'ImVec2';
end

function parse_imvec4(content)
    imgui_types['ImVec4'] = 'ImVec4';
end

function parse_imgui_api(content)
    imgui_apis ={}
    for line in content:gmatch('.-\n') do
        -- print('Parse imgui api','line',line)
        if line:find('IMGUI_API') then
            local rconst, rtype, rdec, fname, args = line:gmatch('IMGUI_API%s+(const%s+)([%w_]*)%s*([*&]?)%s*([%w_]+)(%b())')()
            if not rconst then
                rconst = ''
                rtype, rdec, fname, args = line:gmatch('IMGUI_API%s+([%w_]*)%s*([*&]?)%s*([%w_]+)(%b())')()
            end

            local proto = {}
            proto.fname = fname
            proto.rtype = rtype
            proto.rconst= rconst
            proto.rdec = rdec
            proto.args = args
            table.insert(imgui_apis, proto)
        end
    end

    local all_types = {}
    for i,v in ipairs(imgui_apis) do
        -- print(string.format('func[%s] ret:%s %s%s  args:%s',v.fname,v.rconst, v.rtype,v.rdec, v.args))
        all_types[v.rtype] = true
        print('name', v.fname)
        local arg_result =  parse_funcargs_cap(v.args)
        
    end

    for k,v in pairs(all_types) do
        print(k,v)
    end
    
    for k,v in pairs(imgui_api_arg_types) do
        print('imgui_api_arg_types' ,k,v)
    end

end

function parse_enum_blocks(content)

end


function parse_imgui_header(path)
    local file = io.open(path)
    local content = file:read('a')
    content = content:gsub('//.-\n','\n')   --去掉注释
    content = content:gsub('/%*.-%*/','')   
    content = remove_empty_lines(content)    

    local output_file = io.open([[F:\Github\SimpleEngine\scripts\client\b.txt]],'w')
    for i=1, #imgui_header_separate_flags-1 do
        local begin_str = imgui_header_separate_flags[i][1]
        local end_str = imgui_header_separate_flags[i+1][1]
        local parse_flag = imgui_header_separate_flags[i][2]
        local s = begin_str=='' and 1 or content:find(begin_str)
        local e = content:find(end_str)
        local sub = content:sub(s,e-1)
        content = content:sub(e)

        if parse_flag=='skip' then
        elseif parse_flag =='parse struct' then
            parse_struct(sub)
        elseif parse_flag =='parse typedef' then
            parse_typedef(sub)
        elseif parse_flag =='parse ImVec2' then
            parse_imvec2(sub)
        elseif parse_flag =='parse ImVec4' then
            parse_imvec4(sub)
        elseif parse_flag =='parse ImGuiAPI' then
            parse_imgui_api(sub)
        elseif parse_flag =='parse enum blocks' then
            parse_enum_blocks(sub)
        end

        output_file:write('\nsub :'..parse_flag.. '\n'..sub)
    end
    output_file:close()
end


parse_imgui_header([[F:\Github\SimpleEngine\internals\imgui\include\imgui.h]])








