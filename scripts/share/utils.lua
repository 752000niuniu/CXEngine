
function utils_fetch_sort_keys(tbl, sortfunc)
    local kset = {}
    for k,v in pairs(tbl) do
        table.insert(kset,k)
    end
    table.sort(kset, sortfunc)
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
    local strs = utils_string_split(str,cut)
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


imgui.HorizontalLayout = function(tbl, next_fn, cb, wrap_w)
    local line_width = wrap_w or imgui.GetContentRegionAvailWidth() 
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
                    if col and col.name == key then
                        if col.def then
                            if vals[i] == '' then
                                vals[i] = col.def
                            end
                        end
                        
                        if col.fmt == 'n' then
                            row[key] = tonumber(vals[i])
                        elseif col.fmt == 'i' then
                            row[key] = math.tointeger(vals[i])
                        elseif col.fmt == 'pos' then
                            local v = utils_string_split(vals[i], ',')
                            local pos = {}
                            if #v == 2 then
                                pos.x = math.tointeger(v[1])
                                pos.y = math.tointeger(v[2])
                            else
                                pos.x = 0
                                pos.y = 0
                            end
                            row[key] = pos
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

function utils_parse_tsv_by_main_key(path, columns, main_key)
    local tbl, col_names = utils_parse_tsv(path, columns)
    local ret = {}
    for i, row in ipairs(tbl) do
        ret[row[main_key]] = row
    end
    return ret
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

function traversal_file(path, cb)
	local files, dirs = ks.disk_dir(path)

	for i, v in ipairs(files) do 
		cb(v)
	end
	for i,v in ipairs(dirs) do
		local subpath = path..'/'..v
		traversal_file(subpath, cb)
	end
end

function sort_file_tree_info_once(node)
	if node.nodes then
		table.sort(node.nodes,function(a,b) 
			local alua = a.key:match('%.lua') or a.key:match('%.tab') 
			local blua = b.key:match('%.lua') or b.key:match('%.tab')
			-- ks.log_info(a.key, b.key, alua, blua)
			if alua ~= blua then
				if alua then
					return false
				else
					return true
				end
			else
				return a.key < b.key
			end
		end)
	end
end


function sort_file_tree_info(node)
	if node.nodes then
		table.sort(node.nodes,function(a,b) 
			local alua = a.key:match('%.lua') 
			local blua = b.key:match('%.lua') 
			-- ks.log_info(a.key, b.key, alua, blua)
			if alua ~= blua then
				if alua then
					return false
				else
					return true
				end
			else
				return a.key < b.key
			end
		end)
		for ni,n in ipairs(node.nodes) do
			sort_file_tree_info(n)
		end
	end
end


function get_path_from_node(node)
	local dirs = {}
	local fname = node.key
	while node.p ~= nil do
		if node.p.key ~='' then
			table.insert(dirs,1,node.p.key)
		end
		node = node.p
	end
	local dir = table.concat(dirs,'/')
	local path = dir.. '/'..fname
	-- ks.log_info('path '.. path)
	return path
end




function build_file_tree_info(filepaths)
	local root = {}
	root.key = ''
	root.p = nil
	root.nodes = nil

	for i,path in ipairs(filepaths) do
		filepaths[i] = path:gsub('\\','/')
		local sps = utils_string_split(filepaths[i],'/')
		local cur = root
		for si,s in ipairs(sps) do
			if s:match('%.lua') or s:match('%.tab') then
				local node = {}
				node.key = s 
				node.p = cur 
				node.nodes = nil
				if cur.nodes == nil then
					cur.nodes = {}
				end
				table.insert(cur.nodes, node)
			else
				if cur.nodes == nil then
					cur.nodes = {}
				end
				local found_node = nil
				for ni,n in ipairs(cur.nodes) do
					if n.key == s then
						found_node = n
						break
					end 
				end
				if not found_node then
					local node = {}
					node.key = s 
					node.p = cur 
					node.nodes = nil
					node.lua_cnt = 0
					table.insert(cur.nodes, node)
					cur = node
				else 
					cur = found_node
				end
			end
		end
	end
	sort_file_tree_info_once(root)
	return root
end


local cur_selection = ''
function show_next_tree_node(node)
	for i,n in ipairs(node.nodes) do
		if n.nodes then
			if imgui.TreeNode(n.key.. '##'..tostring(n)) then
				sort_file_tree_info_once(n)
				show_next_tree_node(n)
				imgui.TreePop()
			end
		else
			if imgui.Selectable(n.key..'##'..tostring(n), cur_selection==tostring(n)) then
				cur_selection = tostring(n)
			end
		end
	end
end

function IsServer()
    return process_is_server()
end

function IsClient()
    return not process_is_server()
end

function prop_id_to_name(prop_id)
    local tbl = content_system_get_table('actor_template')
    return tbl[prop_id+1].name
end

function read_database_file(path)
    local file = io.open(path,'r')
    if not file then
        local fw = io.open(path,'w')
        fw:close()
        file = io.open(path,'r')
    end
    local data = file:read('a')
	file:close()
	if data~='' then
		return cjson.decode(data)
	end
end

function format_lua_path(path)
     if string.find(path, '@') == 1 then
        path = string.sub(path, 2)
    end
    local subpaths = {} 
    path = path..'/'
    for subpath in path:gmatch('(.-)[\\/]') do
        if #subpaths == 0 then
            subpath = subpath:lower()
        end
        if subpath == '..' then
            table.remove(subpaths)
        else
            table.insert(subpaths,subpath)
        end
    end
    path = table.concat(subpaths,'\\')
    print(path)
    return path
end


__uids__ = __uids__ or {}
function utils_next_uid(ns)
    if not __uids__[ns] then
        __uids__[ns] = os.time()
    end
    __uids__[ns] = __uids__[ns] + 1
    return __uids__[ns]
end

imgui.TextFormat = function(...)
    return imgui.Text(string.format(...))
end

local proto_to_name = {
    [PTO_C2C_SIGNUP] = 'PTO_C2C_SIGNUP',
    [PTO_C2C_LOGIN] = 'PTO_C2C_LOGIN',
    [PTO_C2C_LOGOUT] = 'PTO_C2C_LOGOUT',
    [PTO_C2C_MOVE_TO_POS] = 'PTO_C2C_MOVE_TO_POS',
    [PTO_C2C_PLAYER_ENTER] = 'PTO_C2C_PLAYER_ENTER',
    [PTO_C2C_ACTOR_ENTER] = 'PTO_C2C_ACTOR_ENTER',
    [PTO_C2C_CHAT] = 'PTO_C2C_CHAT',
    [PTO_C2C_SAVE_ACTORS] = 'PTO_C2C_SAVE_ACTORS',
    [PTO_C2C_SAVE_ACCOUNT_DATABASE] = 'PTO_C2C_SAVE_ACCOUNT_DATABASE',
    [PTO_C2S_COMBAT_START] = 'PTO_C2S_COMBAT_START',
    [PTO_S2C_COMBAT_START] = 'PTO_S2C_COMBAT_START',
    [PTO_S2C_COMBAT_EXECUTE] = 'PTO_S2C_COMBAT_EXECUTE',
    [PTO_C2S_COMBAT_CMD] = 'PTO_C2S_COMBAT_CMD',
    [PTO_S2C_SYNC_PROPS] = 'PTO_S2C_SYNC_PROPS',
    [PTO_C2S_NPC_DIALOG] = 'PTO_C2S_NPC_DIALOG',
    [PTO_S2C_NPC_DIALOG] = 'PTO_S2C_NPC_DIALOG',
    [PTO_C2S_DOSTRING] = 'PTO_C2S_DOSTRING',
    [PTO_C2S_PLAYER_DOSTRING] = 'PTO_C2S_PLAYER_DOSTRING',
    [PTO_C2S_ACTOR_DOSTRING] = 'PTO_C2S_ACTOR_DOSTRING',
    [PTO_C2S_CREATE_PLAYER] = 'PTO_C2S_CREATE_PLAYER',
    [PTO_C2S_CLICK_NPC] = 'PTO_C2S_CLICK_NPC',
    [PTO_S2C_CLICK_NPC] = 'PTO_S2C_CLICK_NPC',
    [PTO_C2S_TEAM_CREATE] = 'PTO_C2S_TEAM_CREATE',
    [PTO_S2C_TEAM_CREATE] = 'PTO_S2C_TEAM_CREATE',
    [PTO_C2S_TEAM_ENTER_TEAM] = 'PTO_C2S_TEAM_ENTER_TEAM',
    [PTO_C2S_TEAM_LEAVE_TEAM] = 'PTO_C2S_TEAM_LEAVE_TEAM',
    [PTO_C2S_TEAM_CHANGE_LEADER] = 'PTO_C2S_TEAM_CHANGE_LEADER',
    [PTO_C2S_TEAM_DISMISS] = 'PTO_C2S_TEAM_DISMISS',
    [PTO_S2C_TEAM_DISMISS] = 'PTO_S2C_TEAM_DISMISS',
    [PTO_C2S_TEAM_ADD_MEMBER] = 'PTO_C2S_TEAM_ADD_MEMBER',
    [PTO_S2C_TEAM_ADD_MEMBER] = 'PTO_S2C_TEAM_ADD_MEMBER',
    [PTO_C2S_TEAM_REMOVE_MEMBER] = 'PTO_C2S_TEAM_REMOVE_MEMBER',
    [PTO_S2C_TEAM_REMOVE_MEMBER] = 'PTO_S2C_TEAM_REMOVE_MEMBER',
    [PTO_C2S_FETCH_TEAM] = 'PTO_C2S_FETCH_TEAM',
    [PTO_S2C_FETCH_TEAM] = 'PTO_S2C_FETCH_TEAM',
    [PTO_C2S_COMBAT_LEAVE_BATTLE] = 'PTO_C2S_COMBAT_LEAVE_BATTLE',
    [PTO_S2C_COMBAT_LEAVE_BATTLE] = 'PTO_S2C_COMBAT_LEAVE_BATTLE',
    [PTO_C2S_COMBAT_END_BATTLE] = 'PTO_C2S_COMBAT_END_BATTLE',
    [PTO_S2C_COMBAT_END_BATTLE] = 'PTO_S2C_COMBAT_END_BATTLE',
    [PTO_C2S_CREATE_SUMMON] = 'PTO_C2S_CREATE_SUMMON',
    [PTO_S2C_CREATE_SUMMON_RESP] = 'PTO_S2C_CREATE_SUMMON_RESP',
    [PTO_C2S_FETCH_SUMMON] = 'PTO_C2S_FETCH_SUMMON',
    [PTO_S2C_FETCH_SUMMON_RESP] = 'PTO_S2C_FETCH_SUMMON_RESP',
    [PTO_C2S_GM] = 'PTO_C2S_GM',
    [PTO_S2C_GM] = 'PTO_S2C_GM',
    [PTO_C2S_SWITCH_SCENE] = 'PTO_C2S_SWITCH_SCENE',
    [PTO_S2C_SWITCH_SCENE] = 'PTO_S2C_SWITCH_SCENE',
    [PTO_C2S_FETCH_ACCOUNT_INFO] = 'PTO_C2S_FETCH_ACCOUNT_INFO',
    [PTO_C2S_COMBAT_CREATE] = 'PTO_C2S_COMBAT_CREATE',
    [PTO_S2C_COMBAT_CREATE] = 'PTO_S2C_COMBAT_CREATE',
    [PTO_C2S_CREATE_ACTOR] = 'PTO_C2S_CREATE_ACTOR',
    [PTO_S2C_CREATE_ACTOR] = 'PTO_S2C_CREATE_ACTOR',
    [PTO_C2S_DELETE_ACTOR] = 'PTO_C2S_DELETE_ACTOR',
    [PTO_S2C_DELETE_ACTOR] = 'PTO_S2C_DELETE_ACTOR',
    [PTO_C2S_SAVE_ACTOR] = 'PTO_C2S_SAVE_ACTOR',
    [PTO_S2C_SAVE_ACTOR] = 'PTO_S2C_SAVE_ACTOR',
    [PTO_C2S_PVE_BATTLE_START] = 'PTO_C2S_PVE_BATTLE_START',
    [PTO_S2C_PVE_BATTLE_START] = 'PTO_S2C_PVE_BATTLE_START',
    [PTO_C2S_COMBAT_PVE_START] = 'PTO_C2S_COMBAT_PVE_START',
    [PTO_S2C_COMBAT_PVE_START] = 'PTO_S2C_COMBAT_PVE_START',
    [PTO_C2S_COMBAT_PVP_START] = 'PTO_C2S_COMBAT_PVP_START',
    [PTO_S2C_COMBAT_PVP_START] = 'PTO_S2C_COMBAT_PVP_START'
}

function proto_name(proto)
    return proto_to_name[proto] or proto
end