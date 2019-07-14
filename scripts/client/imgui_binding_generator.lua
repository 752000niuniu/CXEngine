
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
    local output_file = io.open([[E:\Github\SimpleEngine\scripts\client\a.txt]],'w')
    output_file:write(new_content)
    output_file:close()
    -- print(new_content)
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

local TypeProtoMT = {}
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

function TypeProtoMT:IsRef()
    return self.decor=='&'
end

function TypeProtoMT:IsPtr()
    return self.decor=='*' 
end

function TypeProtoMT:Type()
    local ret = {}
    if self.final and self.final ~= '' then
        table.insert(ret, 'const ')
    end
    table.insert(ret,self.type)
    if self.decor and self.decor ~= '' then
        table.insert(ret,self.decor)
    end
    if self.is_array then
        table.insert(ret,'[')
        if self.array_size then
            table.insert(ret,tostring(self.array_size))
        end
        table.insert(ret,']')
    end
    return table.concat(ret)
end

function TypeProtoMT:ToString()
    local ret = {}
    if self.final and self.final ~= '' then
        table.insert(ret, 'const ')
    end
    table.insert(ret,self.type)
    if self.decor and self.decor ~='' then
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
        final   = final or '',
        decor   =  decor,
        is_array   = is_array,
        array_size = array_size,
        func    = nil,
        def     = nil,
        proto_type = '',
    }
    setmetatable(type_proto, TypeProtoMT)
    return type_proto
end

local FuncProtoMT = {}
function FuncProtoMT:WrapName()
    local tbl = {}
    table.insert(tbl, self.name)
    
    if #self.args > 0 then
        table.insert(tbl, '_'..#self.args..'_')
        for i,arg in ipairs(self.args) do
            if arg.type == 'ImVec2' then
                table.insert(tbl, 'v2')
            elseif arg.type == 'ImVec4' then
                table.insert(tbl, 'v4')
            elseif arg:Type() == 'const char*' then
                table.insert(tbl, 's')
            elseif arg.type == 'int' or arg.type =='unsigned int' or arg.type =='unsigned short' or imgui_typedefs[arg.type] == 'int' or imgui_typedefs[arg.type] == 'unsigned int' then
                if arg.type:find('unsigned') then
                    table.insert(tbl, 'I')
                else
                    table.insert(tbl, 'i')
                end
            elseif arg.type =='float' or arg.type =='double' then
                if arg.type:find('unsigned') then
                    table.insert(tbl, 'N')
                else
                    table.insert(tbl, 'n')
                end
            elseif arg.type == 'bool' then
                table.insert(tbl, 'b')
            else
                table.insert(tbl, 'u')
            end
            if arg.decor=='*' and arg:Type() ~= 'const char*' then
                table.insert(tbl, 'p')
            end
        end
    end
    return table.concat(tbl)
end

function FuncProtoMT:ToString()
    local tstr = {}
    if self.ret then
        table.insert(tstr, self.ret:ToString())
        table.insert(tstr, ' ')
    end

    if self.name~='' then
        table.insert(tstr, self.name)
    end
    table.insert(tstr, '(')
    if #self.args > 0 then
        for i,arg in ipairs(self.args) do
            table.insert(tstr, arg:ToString())
            if i~= #self.args then
                table.insert(tstr, ',')
            end
        end
    end
    table.insert(tstr, ');')
    return table.concat(tstr)
end

function FuncProtoMT:CalcSupported()
    local supported = true
    if self.args then
        for i,arg in ipairs(self.args) do
            -- print('//arg'..i, arg:Type(), arg.name, '=', arg.def or '')
            if not arg:IsSupported()then
                supported = false
                break
            end
            if arg.type == 'ImVec2' or
                arg.type == 'ImVec4' or
                arg:Type() == 'const char*' or
                ((arg.type == 'int' or arg.type =='unsigned int' or arg.type =='unsigned short' or imgui_typedefs[arg.type] == 'int' or imgui_typedefs[arg.type] == 'unsigned int') and (not arg.is_array))  or
                ((arg.type =='float' or arg.type =='double') and (not arg.is_array)) or
                arg.type == 'bool' then
            else 
                supported = false
                break
            end
        end
    end
    self.supported = supported
end

FuncProtoMT.__index = FuncProtoMT
function func_proto_create(name)
    local func_proto = {
        name = name,
        args = {},
        ret  = nil,
        raw_args = '',
        brace_repls = {},
        proto_type= '',
    }
    setmetatable(func_proto, FuncProtoMT)
    return func_proto
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
            
            local proto = func_proto_create(fname)
            proto.ret =  type_proto_create('',rtype, rdec, rconst) 
            proto.raw_args = args
            if args ~= '()' then
                local brace_repls = {}  
                args = args:sub(2,#args-1)
                args = args:gsub('(%b())',function(cap)
                    table.insert(brace_repls, cap)
                    return '@'..#brace_repls
                end)
                proto.brace_repls = brace_repls
                proto.args  = parse_funcargs_cap(args)
            end
            proto:CalcSupported()
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

    imgui_typedefs['size_t'] = 'unsigned int'
    local parsed_skip_file = io.open([[E:\Github\SimpleEngine\scripts\client\b.txt]],'w')
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
        parsed_skip_file:write('\nsub :'..parse_flag.. '\n'..sub)
    end
    parsed_skip_file:close()
    
    local unsupported_func = {}
    for i,proto in ipairs(imgui_apis) do
        print('//'..proto:ToString())
        
        if not proto.supported then
            print('//UnSupported '.. proto.name)
            table.insert(unsupported_func, proto:WrapName())
        else
--[[
    函数生成算法:
    1. c函数原型里, 每有一个arg, 就对应于一个lua的check 或者 toxxx 语句
    2. 读出这些args后, 调用imguiAPI, 得到返回值
    3. 用c的返回类型转换成lua的pushxxx
    4. 打完收工
]]
            local call_api_args = {}
            local fun_impl = {}
            function fun_impl:write_line(fmt, ...) 
                table.insert(fun_impl,string.format(fmt,...))
            end
            fun_impl:write_line('int cximgui_%s(lua_State* L){', proto:WrapName());
            if #proto.args > 0 then
                fun_impl:write_line('\tint __argi__ = 0;');
            end
            local ret_args = {}
            for i,arg in ipairs(proto.args) do
                if arg:IsRef() or (arg:IsPtr() and arg.final=='') then
                    table.insert(ret_args, arg)
                end
                if arg.type == 'ImVec2' then
                    fun_impl:write_line('\t%s %s;', arg.type, arg.name)
                    fun_impl:write_line( '\t%s.x = (float)lua_tonumber(L,++__argi__);' ,arg.name)
                    fun_impl:write_line( '\t%s.y = (float)lua_tonumber(L,++__argi__);' ,arg.name)
                    local call_arg = arg:IsPtr() and ('&'..arg.name) or arg.name
                    table.insert(call_api_args, call_arg)
                elseif arg.type == 'ImVec4' then
                    fun_impl:write_line('\t%s %s;', arg.type, arg.name)
                    fun_impl:write_line( '\t%s.x = (float)lua_tonumber(L,++__argi__);' ,arg.name)
                    fun_impl:write_line( '\t%s.y = (float)lua_tonumber(L,++__argi__);' ,arg.name)
                    fun_impl:write_line( '\t%s.z = (float)lua_tonumber(L,++__argi__);' ,arg.name)
                    fun_impl:write_line( '\t%s.w = (float)lua_tonumber(L,++__argi__);' ,arg.name)
                    local call_arg = arg:IsPtr() and ('&'..arg.name) or arg.name
                    table.insert(call_api_args, call_arg)
                elseif arg:Type() == 'const char*' then
                    fun_impl:write_line( '\t%s %s = lua_tostring(L, ++__argi__);' ,arg:Type(), arg.name)
                    table.insert(call_api_args, arg.name)
                elseif (arg.type == 'int' or arg.type =='unsigned int' or arg.type =='unsigned short' or imgui_typedefs[arg.type] == 'int' or imgui_typedefs[arg.type] == 'unsigned int')  and (not arg.is_array) then
                    fun_impl:write_line( '\t%s %s = (%s)lua_tointeger(L, ++__argi__);' ,arg.type, arg.name, arg.type)
                    local call_arg = arg:IsPtr() and ('&'..arg.name) or arg.name
                    table.insert(call_api_args, call_arg)
                elseif (arg.type =='float' or arg.type =='double') and (not arg.is_array) then
                    fun_impl:write_line( '\t%s %s = (%s)lua_tonumber(L, ++__argi__);' ,arg.type, arg.name , arg.type)
                    local call_arg = arg:IsPtr() and ('&'..arg.name) or arg.name
                    table.insert(call_api_args, call_arg)
                elseif arg.type == 'bool' then
                    fun_impl:write_line( '\t%s %s = lua_toboolean(L, ++__argi__);' ,arg.type, arg.name)
                    local call_arg = arg:IsPtr() and ('&'..arg.name) or arg.name
                    table.insert(call_api_args, call_arg)
                else
                    print('error')
                    break
                end 
            end
            if proto.supported then
                local ret = proto.ret
                local ret_cnt = 1
                if ret.type =='void' then
                    ret_cnt  = 0
                    fun_impl:write_line( '\tImGui::%s(%s);', proto.name, table.concat(call_api_args,','))    
                else
                    fun_impl:write_line( '\t%s __ret__ = ImGui::%s(%s);', ret:Type(), proto.name, table.concat(call_api_args,','))    
                    if ret.type == 'ImVec2' then
                        fun_impl:write_line( '\tlua_pushnumber(L, __ret__.x);')
                        fun_impl:write_line( '\tlua_pushnumber(L, __ret__.y);')
                        ret_cnt = 2
                    elseif ret.type == 'ImVec4' then
                        fun_impl:write_line( '\tlua_pushnumber(L, __ret__.x);')
                        fun_impl:write_line( '\tlua_pushnumber(L, __ret__.y);')
                        fun_impl:write_line( '\tlua_pushnumber(L, __ret__.z);')
                        fun_impl:write_line( '\tlua_pushnumber(L, __ret__.w);')
                        ret_cnt = 4
                    elseif ret:Type() == 'const char*' then
                        fun_impl:write_line( '\tlua_pushstring(L, __ret__);')
                    elseif ret.type== 'int' or ret.type =='unsigned int' or ret.type =='unsigned short' or imgui_typedefs[ret.type] == 'int'   or imgui_typedefs[ret.type] == 'unsigned int' then
                        fun_impl:write_line( '\tlua_pushinteger(L, __ret__);')
                    elseif ret.type =='float' or ret.type =='double' then
                        fun_impl:write_line( '\tlua_pushnumber(L, __ret__);')
                    elseif ret.type == 'bool' then
                        fun_impl:write_line( '\tlua_pushboolean(L, __ret__);')
                    else 
                        ret_cnt = 0
                    end
                end

                for i,ret_arg in ipairs(ret_args) do
                    if ret_arg.type == 'ImVec2' then
                        fun_impl:write_line( '\tlua_pushnumber(L, %s.x);', ret_arg.name)
                        fun_impl:write_line( '\tlua_pushnumber(L, %s.y);', ret_arg.name)
                        ret_cnt = ret_cnt + 2
                    elseif ret_arg.type == 'ImVec4' then
                        fun_impl:write_line( '\tlua_pushnumber(L, %s.x);', ret_arg.name)
                        fun_impl:write_line( '\tlua_pushnumber(L, %s.y);', ret_arg.name)
                        fun_impl:write_line( '\tlua_pushnumber(L, %s.z);', ret_arg.name)
                        fun_impl:write_line( '\tlua_pushnumber(L, %s.w);', ret_arg.name)
                        ret_cnt = ret_cnt + 4
                    elseif ret_arg.type== 'int' or ret.type =='unsigned int' or ret.type =='unsigned short' or imgui_typedefs[ret.type] == 'int'  or imgui_typedefs[ret.type] == 'unsigned int' then
                        fun_impl:write_line( '\tlua_pushinteger(L, %s);', ret_arg.name)
                        ret_cnt = ret_cnt + 1
                    elseif ret_arg.type =='float' or ret.type =='double' then
                        fun_impl:write_line( '\tlua_pushnumber(L, %s);', ret_arg.name)
                        ret_cnt = ret_cnt + 1
                    elseif ret_arg.type == 'bool' then
                        fun_impl:write_line( '\tlua_pushboolean(L, %s);', ret_arg.name)
                        ret_cnt = ret_cnt + 1
                    end
                end
                fun_impl:write_line( '\treturn %d;', ret_cnt)
                table.insert(fun_impl, '};')
                
                local imp = table.concat(fun_impl,'\n')
                print(imp)
                print('')
            end
        end
    end
    print('\n//total func', #imgui_apis, 'unSupported', #unsupported_func)


    print('luaL_Reg cximgui_methods[] = {')
    local last_name = ''
    for i,proto in ipairs(imgui_apis) do
        if proto.supported and last_name ~= proto.name then
            print(string.format('\t{"%s",%s},', proto.name,'cximgui_'.. proto:WrapName()) )    
        end
        last_name = proto.name
    end 
    print('\t{ NULL, NULL }')
    print('};')

    print([[//open_imgui
void luaopen_cximgui(lua_State* L){
    if (luaL_newmetatable(L, "mt_cximgui")) {
        luaL_setfuncs(L, cximgui_methods , 0);
        lua_setfield(L, -1, "__index");
    }
    else {
        std::cout << "associate cximgui error!" << std::endl;
    }

    lua_newtable(L);
	luaL_setmetatable(L, "mt_cximgui");
	lua_setglobal(L,"imgui");
}


]])

end


parse_imgui_header([[E:\Github\SimpleEngine\internals\imgui\include\imgui.h]])


