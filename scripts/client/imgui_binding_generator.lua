local file = io.open([[E:\gjol\game\server\puss_imgui\imgui\imgui.h]])
local content = file:read('a')
content = content:gsub('//.-\n','\n')
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
content = remove_empty_lines(content)

header_separate_flags = {
    [[struct ImDrawChannel;]],
    [[typedef unsigned int ImGuiID;]],
    [[struct ImVec2]],
    [[struct ImVec4]],
    [[namespace ImGui]],
    [[enum ImGuiWindowFlags_]],
    [[#define IMGUI_PAYLOAD_TYPE_COLOR_3F     "_COL3F"]],
    [[enum ImGuiDataType_]],
    [[struct ImGuiStyle]],
    [[#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS]],
    [[#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS]],
    [[namespace ImGui]],
    [[#endif]],
    [[enum ImDrawCornerFlags_]],
    [[struct ImDrawList]],
    [[enum ImFontAtlasFlags_]],
    [[struct ImFontAtlas]],
    [[enum ImGuiViewportFlags_]],
    [[struct ImGuiViewport]],
    [[#endif]]
}


header_parse_flags = {
    'skip',
    'parse struct',
    'parse typedef',
    'parse ImVec2',
    'parse ImVec4',
    'parse ImGuiAPI',
    'parse enum blocks',
    'skip',
    'parse enum blocks',
    'skip', 
    'skip',
    'skip',
    'parse ImGuiAPI',
    'skip',
    'parse enum blocks',
    'skip',
    'parse enum blocks',
    'skip',
    'parse enum blocks',
    'skip',
    'skip'
}

function parse_imgui_header(content)
    -- local buf = {}
    -- for c in content:gmatch('(.)') do
    --     local str = table.concat(buf)
    --     if str:find([[struct ImDrawChannel;]]) then
    --         buf = {}
    --         print(str)
    --         return
    --     else 
    --         table.insert(buf,c)
    --     end
    -- end
    -- print(table.concat(line))
    local buf = content

    


end

-- table.insert(buf,c)
parse_imgui_header(content)








