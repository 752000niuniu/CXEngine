function OnSceneInit()

end

function OnSceneUpdate()
         
end
function OnSceneImGuiUpdate()
    if imgui.Button('GetWas') then
        res_get_was(MHIMAGEWDF,0X00720083,vfs_makepath('a'))
    end
end

function OnSceneDraw()

end