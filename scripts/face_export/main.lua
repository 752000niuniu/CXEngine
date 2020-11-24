--[[
CXEngine.exe --cwd=[工作目录] --script_path=[lua文件夹相对工作目录]/  --luapath=[相对script_path下的lua文件路径]
示例：
./bin/Debug/CXEngine.exe --cwd=[CXEngine目录的路径]/  --script_path=scripts/examples/ --luapath=example0.lua

example0 最简单的教程 显示一个窗口
]] --
-- update函数在iw_render里面被调用，游戏的循环是在底层的cpp代码里，底层每一帧会调用一次你传入给iw_render的lua函数。

script_system_dofile('../share/utils.lua')
script_system_dofile('../share/vfs.lua')

recordPathSB = recordPathSB or imgui.CreateStrbuf('D:/', 512)
recordHeaderInfo = ''

local batchExportAnimDirSB = batchExportAnimDirSB or imgui.CreateStrbuf('', 512)
local exportAnimFacePathSB = exportAnimFacePathSB or imgui.CreateStrbuf('', 512)
local exportAnimFbxSB = exportAnimFbxSB or imgui.CreateStrbuf('', 512)
local exportAnimSavePathSB = exportAnimSavePathSB or imgui.CreateStrbuf('', 512)

function ImDrawRecordViewer()
    if imgui.Button('查看##RecordViewer') then
        local path = recordPathSB:str()
        if is_file_exist(path) then
            recordHeaderInfo = read_record_file_header(path)
        end
    end
    imgui.SameLine()
    imgui.DropInputText('RecordPath', recordPathSB, 300)

    imgui.Text(recordHeaderInfo)
end

function update()
    if imgui.Button("导出Animation") then
        imgui.OpenPopup("ExportAnimationPopup")
    end

    if imgui.BeginPopup("ExportAnimationPopup") then
        imgui.DropInputText('表情录制文件路径', exportAnimFacePathSB)
        imgui.DropInputText('模型FBX文件路径', exportAnimFbxSB)
        imgui.DropInputText('导出FBX文件路径', exportAnimSavePathSB)
        if imgui.Button('导出') then
            local fbxname = exportAnimFbxSB:str():match('.+[\\/](.+)%.fbx')
            -- local savepath = vfs_makepath('res\\' .. fbxname .. '_anim.fbx')
            local savepath = exportAnimSavePathSB:str()
            local savedir = savepath:match('(.+)[\\/]')
            savedir = util_utf8_to_gb2312(savedir)
            local exepath = vfs_makepath('bin/Animation.exe')
            local cmd = 'start ' .. exepath .. ' --exportby=record_face' ..
                            ' --recpath=' .. exportAnimFacePathSB:str() ..
                            ' --fbxpath=' .. exportAnimFbxSB:str() ..
                            ' --savepath=' .. savepath .. ' --animname=anim '
            
            cxlog_info(cmd)
            os.execute(cmd)

            os.execute('start ' .. savedir)
            imgui.CloseCurrentPopup()
        end
        imgui.EndPopup()
    end

    if imgui.Button("批量导出Animation") then
        imgui.OpenPopup("BatchExportAnimationPopup")
    end

    if imgui.BeginPopup("BatchExportAnimationPopup") then
        imgui.PushItemWidth(300)
        imgui.DropInputText('目录', batchExportAnimDirSB)
        imgui.PopItemWidth()
        imgui.SameLine()
        if imgui.Button('导出') then
            local dir = batchExportAnimDirSB:str()
            dir = util_utf8_to_gb2312(dir)
            if dir:sub(-1) ~= '\\' and dir:sub(-1) ~= '/' then
                dir = dir .. '/'
            end
            local tsvPath = dir .. 'config.tsv'
            local tbl = utils_parse_tsv_file(dir .. tsvPath, {
                {name = 'fbx'}, {name = 'face'}, {name = 'export'}
            })
            for k, v in pairs(tbl) do
                local fbxpath = dir .. '/fbx/' .. v.fbx
                local recpath = dir .. '/face/' .. v.face
                local savepath = dir .. '/export/' .. v.export
                local exepath = vfs_makepath('bin/Animation.exe')
                local cmd = 'start ' .. exepath .. ' --exportby=record_face' ..
                                ' --recpath=' .. recpath .. ' --fbxpath=' ..
                                fbxpath .. ' --savepath=' .. savepath ..
                                ' --animname=anim '
                os.execute(cmd)
            end
            os.execute('start ' .. dir)
            imgui.CloseCurrentPopup()
        end
        imgui.EndPopup()
    end

end

do
    iw_init(800, 600) -- iw_init为c函数,初始化了一个800x600的窗口,lua第一行代码从这里执行.
    iw_set_font(vfs_get_workdir() .. '/res/font/simsun.ttc')
    iw_render(update) -- iw_render为c函数,接受一个lua函数作为参数传入,iw_render执行后,c代码里就进入了游戏循环逻辑,每帧调用传入的lua函数一次.
    iw_deinit() -- iw_deinit为c函数,释放c里面分配的资源,程序走到这里,说明游戏循环已经被跳出.
end
