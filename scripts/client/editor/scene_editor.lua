
local select_scene_name = 0
local str_templ_name = imgui.CreateStrbuf('', 512)
local  copy_actor_type = ACTOR_TYPE_PLAYER
 
local cbx_draw_mask = true
local cbx_draw_strider = true
local cbx_draw_cell = false
local cbx_draw_map  = true
local cbx_draw_announcement = true
local cbx_auto_run = false

function on_scene_editor_update()
    imgui.Begin('SceneEditor')
    local mx, my = imgui.GetMousePos()
	local cur_x ,cur_y = imgui.GetCursorScreenPos()
	if imgui.IsMousePosValid(mx,my) then
		mx = mx - cur_x
		my = my - cur_y
    end
    
    local mapx, mapy = util_screen_pos_to_map_pos(mx,my)
    
    imgui.Text(string.format("[mouse] : x=%.0f,y=%.0f world.x=%.0f,world.y=%.0f",
        mx,my, mapx,mapy))

    local player = scene_manager_fetch_local_player()
    if player then 
        imgui.Text(string.format("[player] : x=%f,y=%f,dir=%f", player:GetX(), player:GetY(), player:GetDir()))
    end
    imgui.SameLine()
    
    local res 
    res, cbx_draw_map = imgui.Checkbox('DrawMap', cbx_draw_map)
    imgui.SameLine()
    res, cbx_draw_cell = imgui.Checkbox('DrawCell', cbx_draw_cell)
    imgui.SameLine()
    res, cbx_draw_strider = imgui.Checkbox('DrawStrider', cbx_draw_strider)
    imgui.SameLine()
    res, cbx_draw_mask = imgui.Checkbox('DrawMask', cbx_draw_mask)
    imgui.SameLine()
    res, cbx_draw_announcement = imgui.Checkbox('DrawAnnouncement', cbx_draw_announcement)
    imgui.SameLine()
    res, cbx_auto_run = imgui.Checkbox('AutoRun', cbx_auto_run)
    
    scene_manager_sync_draw_cbx(cbx_draw_map,cbx_draw_cell,cbx_draw_strider,cbx_draw_mask,cbx_draw_announcement,cbx_auto_run)

    if imgui.CollapsingHeader('CHSceneList') then
        res, select_scene_name  = imgui.ListBox('##SceneList', select_scene_name  , scene_list_name)
        imgui.SameLine()
        if imgui.Button('SwitchSceneByName') then
            scene_manager_switch_scene_by_name(select_scene_name)
        end
    end

    for i=1,10 do
        if imgui.Button('Player'..i) then
            scene_manager_set_player_by_index(i)
        end
        imgui.SameLine()
        if i == 10 then
            if imgui.Button('Clear') then
                scene_manager_set_player_by_index(0)
            end
        end
    end

    imgui.Text('TemplName  :')
    imgui.SameLine()
    imgui.InputText('##temp_name', str_templ_name)
    if imgui.Button('召唤Avatar') then
        scene_add_player_by_templ_name(str_templ_name:str(), copy_actor_type)
    end
    imgui.SameLine()
    res ,copy_actor_type = imgui.SliderInt('##actor_type',copy_actor_type, 1,3)

    if imgui.CollapsingHeader('PlayAction') then
        for i=0,action_system_get_action_size()-1 do
            if imgui.Button(action_system_get_action_name(i)) then
                if player then
                    player:SetActionID(i)
                end
            end
            if i~= action_system_get_action_size()-1 then
                imgui.SameLine()
            end
        end
    end

    if imgui.Button('SwitchToLogin') then
        scene_manager_switch_scene_by_name('TestNetScene')
    end

    if imgui.Button('SwitchToBattle') then
        scene_manager_switch_scene_by_name('BattleScene')
    end
    imgui.End()

end