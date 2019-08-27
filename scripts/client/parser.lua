
function parse_magic_wdf(path)
    local file = io.open(path)
    local content = file:read('a')
    file:close()

    local lines = {}
    for line in content:gmatch('(.-)\n') do
        table.insert(lines,line)
    end
    table.remove(lines,1)
    
    
    local new_lines = {}
    for i,v in ipairs(lines) do
        local strs = utils_string_split(v,'=')
        local new_line = {}
        new_line.wasid = strs[1]
        new_line.name = strs[2]
        table.insert(new_lines,new_line)
    end

    table.sort(new_lines,function(a,b) return a.name < b.name end)
    for i,v in ipairs(new_lines) do
        v.ID = i
    end

    local f_magic_tsv = io.open(vfs_get_tablepath('magic.tsv'),'w+')
    f_magic_tsv:write([[ID	name	wasid]]..'\n')
    for i,v in ipairs(new_lines) do
        f_magic_tsv:write(v.ID..'\t'..v.name..'\t'..MAGICWDF..'-'..v.wasid..'\n')
    end
    f_magic_tsv:close()
end

parse_magic_wdf(vfs_get_tablepath('magic.wdf.ini'))