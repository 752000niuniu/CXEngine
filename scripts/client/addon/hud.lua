


function ui_hud_update()
    local player = actor_manager_fetch_local_player()
    if not player then return end

    local w = imgui.GetWindowSize()
    
    imgui.SetCursorPos(w-200)
    imgui.BeginGroup()
        imgui.PushStyleColor2(ImGuiCol_Text,1,0,0,1)
        imgui.Button('HP:'.. math.floor(player:GetProperty(PROP_HP)) .. '/'..player:GetMaxHP())
        imgui.PopStyleColor()

        imgui.PushStyleColor2(ImGuiCol_Text, 0,1,0,1)
        imgui.Button('MP:' .. math.floor(player:GetProperty(PROP_MP)) ..'/' ..player:GetMaxMP())
        imgui.PopStyleColor()

        imgui.PushStyleColor2(ImGuiCol_Text, 0.8,0.8,0.8,1)
        imgui.Button('SP:'..player:GetProperty(PROP_SP) ..'/150')
        imgui.PopStyleColor()

    imgui.EndGroup()

end