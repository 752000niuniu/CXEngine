
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
local imgui_typedefs = {}
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

--[[
IMGUI_API bool          Combo(const char* label, int* current_item, const char* const items[], int items_count, int popup_max_height_in_items = -1);
IMGUI_API bool          Combo(const char* label, int* current_item, const char* items_separated_by_zeros, int popup_max_height_in_items = -1);      
IMGUI_API bool          ListBox(const char* label, int* current_item, bool (*items_getter)(void* data, int idx, const char** out_text), void* data, int items_count, int height_in_items = -1);
IMGUI_API bool          Combo(const char* label, int* current_item, bool(*items_getter)(void* data, int idx, const char** out_text), void* data, int items_count, int popup_max_height_in_items = -1);
]]--
local TypeProtoMT = {}
function TypeProtoMT:GenLuaPopCode(i, repls)
    
end

function TypeProtoMT:GenLuaPushCode(i, repls)

end

function TypeProtoMT:IsFunc()
    local cnt = 0
    self:ToString():gsub('@',function(cap)
        cnt  = cnt + 1 
        return cap
    end)
    return cnt == 2
end

function TypeProtoMT:IsVoid()
    return  self.type:find('void')
end

function TypeProtoMT:IsSupported()
    return (not self:IsFunc()) and( not self:IsVoid()  )
end

function TypeProtoMT:Type()
    local ret = {}
    if self.final then
        table.insert(ret, 'const ')
    end
    table.insert(ret,self.type)
    if self.decor then
        table.insert(ret,self.decor)
    end
    return table.concat(ret)
end

function TypeProtoMT:ToString()
    local ret = {}
    if self.final then
        table.insert(ret, 'const ')
    end
    table.insert(ret,self.type)
    if self.decor then
        table.insert(ret,self.decor)
    end
    table.insert(ret,' ')
    table.insert(ret,self.name)
    if self.is_array then
        if self.array_size > 0 then
            table.insert(ret,string.format('[%d]', self.array_size ))
        else
            table.insert(ret,string.format('[]'))
        end
    end
    return table.concat(ret)
end
TypeProtoMT.__index = TypeProtoMT
function type_proto_create(name, type, decor, final, is_array, array_size)
    local type_proto = {
        name    = name, 
        type    = type,
        final   = final,
        decor   =  decor,
        is_array   = is_array,
        array_size = array_size,
        func    = nil,
        def     = nil
    }
    setmetatable(type_proto, TypeProtoMT)
    return type_proto
end

function parse_type(str)
    if str =='...' then
        return type_proto_create('', '...', '')
    end
    local final
    local fs,fe = str:find('^%s*const')
    if fs then
        final = 'const'
        str = str:sub(fe+1)
    end

    local unsigned
    fs,fe = str:find('^%s*unsigned')
    if fs then
        unsigned = 'unsigned'
        str = str:sub(fe+1)
    end

    local type, decor, name = str:gmatch('([%w_]+)%s*([&*])%s*([@%w_]+)')()
    if not decor then
        type, name = str:gmatch('([%w_]+)%s+([@%w_]+)')()
    end
    if unsigned then
        type = unsigned..' '..type
    end
    
    -- print('final', final, 'type', type, 'decor', decor, 'name', name)
    fs, fe = str:find(name)
    str = str:sub(fe+1)
    local is_array = false 
    local array_size = 0
    if str:find('%[%d*%]') then
        is_array = true
        local sz = str:gmatch('%[(%d+)%]')()
        if sz then
            array_size = math.tointeger(sz)
        end
    end

    return type_proto_create(name, type, decor, final, is_array, array_size)
end

function parse_funcargs_cap(args)
    args = args..','
    local all_args = {}
    for arg_block in args:gmatch('(.-),') do
        arg_block = arg_block:gsub('@', ' @', 1)    

        if arg_block:find('%.%.%.') then
            local arg = parse_type('...')
            table.insert(all_args, arg)
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
            
            local arg = parse_type(equal_left)
            arg.def = equal_right
            table.insert(all_args, arg)
        end
    end
    return all_args
end

function parse_typedef(content)
    for line in content:gmatch('(.-)\n') do
        -- print('parse_typedef', line)
        if line:find('typedef') and line:find('ImS64')==nil and line:find('ImU64')==nil then
            if line:find('typedef.+%b()%s*%b();') then
                local ret_type, ret_dec, fname_cap, args_cap = line:gmatch('typedef%s*([%w_]+)%s*([&*]?)%s*(%b())(%b());')()
                local fname = fname_cap:gmatch('[*]%s*([%w_]+)')()
                -- print('find function', ret_type, ret_dec, fname, args)             
                -- imgui_typedefs[fname] =  ret_type..ret_dec..' (*)'..args_cap
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
                    imgui_typedefs[last] = table.concat(words,' ')    
                end
            end
        end
    end
end

function parse_imvec2(content)
    imgui_typedefs['ImVec2'] = 'ImVec2';
end

function parse_imvec4(content)
    imgui_typedefs['ImVec4'] = 'ImVec4';
end

local imgui_api_ignore_fnames={
    --[ImGuiStyle]
    'GetStyle',
    'ShowStyleEditor',
    'StyleColorsDark',
    'StyleColorsClassic',
    'StyleColorsLight',
   
    'CreateContext',
    'DestroyContext',
    'GetCurrentContext',
    'SetCurrentContext',
    'GetIO',
    'GetDrawData',
    'GetWindowDrawList',
    'GetWindowViewport',
    'SetNextWindowSizeConstraint',
    'PushFont',
    'PopFont',
    'GetFont',
    'Combo',
    'DragScalar',
    'DragScalarN',
    'SliderScalar',
    'SliderScalarN',
    'VSliderScalar',
    'InputScalar',
    'InputScalarN',
    'ListBox',
    'PlotLines',
    'PlotHistogram',
    'DockSpace',
    'DockSpaceOverViewport',
    'SetNextWindowDockId',
    'SetNextWindowDockFamily',
    'SetDragDropPayload',
    'EndDragDropSource',
    'BeginDragDropTarget',
    'AcceptDragDropPayload',
    'EndDragDropTarget',
    'GetDragDropPayload',
    'GetOverlayDrawList',
    'GetDrawListSharedData',
    'GetStateStorage',
    'SetStateStorage',
    'SetAllocatorFunctions',
    'MemAlloc',
    'MemFree',
    'GetPlatformIO',
    'GetMainViewport',
    'RenderPlatformWindowsDefault',
    'FindViewportByPlatformHandle'
}

local imgui_api_unsupported_types = {
    'void*',        --translate void* to int 
    'void',
    '...',          --translate ... into unsupported , translate const char* fmt, va_list args to const char* string
    'const char* const items[]',
    'items_getter'
}   
   
function parse_imgui_api(content)
    imgui_apis = {}
    for line in content:gmatch('.-\n') do
        if line:find('IMGUI_API') then
            local rconst, rtype, rdec, fname, args = line:gmatch('IMGUI_API%s+(const%s+)([%w_]*)%s*([*&]?)%s*([%w_]+)(%b())')()
            if not rconst then
                rconst = ''
                rtype, rdec, fname, args = line:gmatch('IMGUI_API%s+([%w_]*)%s*([*&]?)%s*([%w_]+)(%b())')()
            end
            
            local proto = {}
            proto.fname = fname
            proto.ret =  type_proto_create('',rtype, rdec, rconst) 
            proto.raw_args = args
            if args == '()' then
                proto.args = {}
                proto.brace_repls = {}
            else
                local brace_repls = {}  
                args = args:sub(2,#args-1)
                args = args:gsub('(%b())',function(cap)
                    table.insert(brace_repls, cap)
                    return '@'..#brace_repls
                end)
                proto.brace_repls = brace_repls
                proto.args  = parse_funcargs_cap(args)
            end
            table.insert(imgui_apis, proto)
        end
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

    for type,define in pairs(imgui_typedefs) do
        print(type,'|',define)
    end

    local unsupported_func = {}
    for i,proto in ipairs(imgui_apis) do
        print(i)
        print('proto', proto.ret:ToString() .. ' '..proto.fname..proto.raw_args)
        print('ret', proto.ret:Type())
        local supported = true
        if proto.args then
            for i,arg in ipairs(proto.args) do
                print('arg'..i, arg:Type(), arg.name, '=', arg.def or '')
                if not arg:IsSupported()then
                    supported = false
                end
            end
        end
        if not supported then
            print('UnSupported '.. proto.fname)
            table.insert(unsupported_func, proto.fname)
        else
--[[
    函数生成算法:
    1. c函数原型里, 每有一个arg, 就对应于一个lua的check 或者 toxxx 语句
    2. 读出这些args后, 调用imguiAPI, 得到返回值
    3. 用c的返回类型转换成lua的pushxxx
    4. 打完收工
]]
            local ret = proto.ret
            
            local fun_impl = {}
            function fun_impl:write_line(fmt, ...) 
                table.insert(fun_impl,string.format(fmt,...))
            end
            fun_impl:write_line('int %s(lua_State* L){', proto.fname);
            fun_impl:write_line('int __argi__ = 0;');
            fun_impl:write_line('%s __ret__;', ret:Type())

            local supported = true
            for i,arg in ipairs(proto.args) do
                if arg.type == 'ImVec2' then
                elseif arg.type == 'ImVec4' then
                elseif arg:Type() == 'const char*' then
                elseif arg.type == 'int' or arg.type =='unsigned int' or arg.type =='unsigned short' or imgui_typedefs[arg.type] == 'int' then
                elseif arg.type =='float' or arg.type =='double' then
                elseif arg.type == 'bool' then
                else
                    supported = false
                    break
                end
                fun_impl:write_line('%s %s;', arg:Type(), arg.name)
            end

            local call_api_args = {}
            for i,arg in ipairs(proto.args) do
                if arg.type == 'ImVec2' then
                    fun_impl:write_line( '%s.x = lua_tonumber(L,__argi__++);' ,arg.name)
                    fun_impl:write_line( '%s.y = lua_tonumber(L,__argi__++);' ,arg.name)
                    table.insert(call_api_args, arg.name)
                elseif arg.type == 'ImVec4' then
                    fun_impl:write_line( '%s.x = lua_tonumber(L,__argi__++);' ,arg.name)
                    fun_impl:write_line( '%s.y = lua_tonumber(L,__argi__++);' ,arg.name)
                    fun_impl:write_line( '%s.z = lua_tonumber(L,__argi__++);' ,arg.name)
                    fun_impl:write_line( '%s.w = lua_tonumber(L,__argi__++);' ,arg.name)
                    table.insert(call_api_args, arg.name)
                elseif arg:Type() == 'const char*' then
                    fun_impl:write_line( '%s = lua_tostring(L, __argi__++);' ,arg.name)
                    table.insert(call_api_args, arg.name)
                elseif arg.type == 'int' or arg.type =='unsigned int' or arg.type =='unsigned short' or imgui_typedefs[arg.type] == 'int' then
                    fun_impl:write_line( '%s = lua_tointeger(L, __argi__++);' ,arg.name)
                    table.insert(call_api_args, arg.name)
                elseif arg.type =='float' or arg.type =='double' then
                    fun_impl:write_line( '%s = lua_tonumber(L, __argi__++);' ,arg.name)
                    table.insert(call_api_args, arg.name)
                elseif arg.type == 'bool' then
                    fun_impl:write_line( '%s = lua_toboolean(L, __argi__++);' ,arg.name)
                    table.insert(call_api_args, arg.name)
                end
            end
            
            fun_impl:write_line( '__ret__ = %s(%s);' , proto.fname, table.concat(call_api_args,','))
            local ret_cnt = 1
            if ret.type == 'ImVec2' then
                fun_impl:write_line( 'lua_pushnumber(L, __ret__.x);')
                fun_impl:write_line( 'lua_pushnumber(L, __ret__.y);')
                ret_cnt = 2
            elseif ret.type == 'ImVec4' then
                fun_impl:write_line( 'lua_pushnumber(L, __ret__.x);')
                fun_impl:write_line( 'lua_pushnumber(L, __ret__.y);')
                fun_impl:write_line( 'lua_pushnumber(L, __ret__.z);')
                fun_impl:write_line( 'lua_pushnumber(L, __ret__.w);')
                ret_cnt = 4
            elseif ret:Type() == 'const char*' then
                fun_impl:write_line( 'lua_pushstring(L, __ret__);')
            elseif ret.type== 'int' or ret.type =='unsigned int' or ret.type =='unsigned short' or imgui_typedefs[ret.type] == 'int' then
                fun_impl:write_line( 'lua_pushinteger(L, __ret__);')
            elseif ret.type =='float' or ret.type =='double' then
                fun_impl:write_line( 'lua_pushnumber(L, __ret__);')
            elseif ret.type == 'bool' then
                fun_impl:write_line( 'lua_pushboolean(L, __ret__);')
            end

            fun_impl:write_line( 'return %d;', ret_cnt)
            table.insert(fun_impl, '};')

            print('impl', proto.fname)
            local imp = table.concat(fun_impl,'\n')
            print( imp)
        end
    end
    print('total func', #imgui_apis, 'unSupported', #unsupported_func)
end


parse_imgui_header([[F:\Github\SimpleEngine\internals\imgui\include\imgui.h]])



