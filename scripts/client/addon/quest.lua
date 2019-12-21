local ui_is_show_quest = false
function ui_toggle_show_quest()
    ui_is_show_quest = not ui_is_show_quest
    return ui_is_show_quest
end


function ui_show_quest()
    if not ui_is_show_quest then return end
    local player = actor_manager_fetch_local_player()
    if not player then return end
    imgui.Begin('Quest')
    imgui.Text('你暂时没有任务！')
    imgui.End()
end
