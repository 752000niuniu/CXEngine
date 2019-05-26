

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
    local output_file = io.open([[E:\Github\SimpleEngine\scripts\client\a.txt]],'w')
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
    { [[namespace ImGui]],                  'parse ImGuiAPI'},
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


function parse_imgui_header(path)
    local file = io.open(path)
    local content = file:read('a')
    content = content:gsub('//.-\n','\n')   --去掉注释
    content = content:gsub('/%*.-%*/','')   
    content = remove_empty_lines(content)    

    local output_file = io.open([[E:\Github\SimpleEngine\scripts\client\b.txt]],'w')
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
        elseif parse_flag =='parse typedef' then
        elseif parse_flag =='parse ImVec2' then
        elseif parse_flag =='parse ImVec4' then
        elseif parse_flag =='parse ImGuiAPI' then
        elseif parse_flag =='parse enum blocks' then
        end

        output_file:write('\nsub :'..parse_flag.. '\n'..sub)
    end
    output_file:close()
end


parse_imgui_header([[E:\Github\SimpleEngine\internals\imgui\include\imgui.h]])








