local select_scene_name = 0
local str_templ_name = imgui.CreateStrbuf('', 512)
local copy_actor_type = ACTOR_TYPE_PLAYER
 
local cbx_draw_mask = true
local cbx_draw_strider = true
local cbx_draw_cell = false
local cbx_draw_map  = true
local cbx_draw_announcement = true

local checkbox_names = {
    {   name = 'DrawMap', cb  = function(v) v.res,cbx_draw_map = imgui.Checkbox(v.name, cbx_draw_map) end },
    {   name = 'DrawCell', cb = function(v) v.res,cbx_draw_cell = imgui.Checkbox(v.name, cbx_draw_cell) end },
    {   name = 'DrawStrider', cb = function(v) v.res,cbx_draw_strider = imgui.Checkbox(v.name, cbx_draw_strider) end },
    {   name = 'DrawMask', cb = function(v) v.res,cbx_draw_mask = imgui.Checkbox(v.name, cbx_draw_mask) end },
    {   name = 'DrawAnnouncement', cb = function(v) v.res,cbx_draw_announcement = imgui.Checkbox(v.name, cbx_draw_announcement) end},
}

function on_scene_editor_update()
    local player = actor_manager_fetch_local_player()
    if not player then return end
    imgui.Begin('场景编辑器')

    local mx, my = imgui.GetMousePos()
    local cur_x ,cur_y = imgui.GetCursorScreenPos()
	if imgui.IsMousePosValid(mx,my) then
		mx = mx - cur_x
		my = my - cur_y
    end

    if imgui.CollapsingHeader('PosInfo') then
        local mapx, mapy = util_screen_pos_to_map_pos(mx,my)
        local px,py = player:GetPos()
        imgui.Text(string.format("[mouse] : x=%.0f,y=%.0f world.x=%.0f,world.y=%.0f",mx,my, mapx,mapy))
        imgui.Text(string.format("[player] : x=%f,y=%f,dir=%f", px,py, player:GetDir()))
    end

    if imgui.CollapsingHeader('MapDrawOption') then
        imgui.HorizontalLayout(checkbox_names,next,function(k,v) v:cb() end)
    end
    scene_manager_sync_draw_cbx(cbx_draw_map,cbx_draw_cell,cbx_draw_strider,cbx_draw_mask,cbx_draw_announcement,cbx_auto_run)

    if imgui.CollapsingHeader('Maps') then
        local maps  = content_system_get_table('maps')
        imgui.HorizontalLayout(maps,next,function(k,v) 
            if imgui.Button(v.name) then
                scene_manager_switch_scene_by_name(v.name)    
            end
        end)
    end  

    if imgui.CollapsingHeader('NPCTemplate') then
        local npc_tbl = content_system_get_table('npc')
        imgui.HorizontalLayout(npc_tbl,next,function(k,v) 
            if imgui.Button(v.ID) then
                local actor = actor_manager_create_player(os.time())
                actor:SetProperty(PROP_ACTOR_TYPE,ACTOR_TYPE_SUMMON)
                actor:SetProperty(PROP_AVATAR_ID, v.ID)
                actor:SetProperty(PROP_WEAPON_AVATAR_ID,'')
                actor:SetPos(player:GetPos())
                actor:SetProperty(PROP_SCENE_ID, player:GetProperty(PROP_SCENE_ID))
                actor:SetDir(player:GetDir())
                -- actor:ModifyHP(100)
                actor:SetProperty(PROP_HP,100) 
                -- actor:(PROP_SCENE_ID, player:GetProperty(PROP_SCENE_ID))
            end
        end)
    end  

    if imgui.CollapsingHeader('PlaySound') then
        local files = vfs_list_files(vfs_makepath([[res\data\sound.wdf.ResFiles]]) )
        local map3 = vfs_list_files(vfs_makepath([[res\data\music.wdf.ResFiles]]))
        
        for i,v in ipairs(map3) do
            table.insert(files,v)
        end
        table.insert(files,1,[[e:\CloudMusic\BIGBANG - La La La.mp3]])
        
        imgui.HorizontalLayout(files,next,function(k,v) 
            -- cxlog_info(k,v)
            if imgui.Button(v) then
                audio_manager_play(v)
            end
        end)
    end

    imgui.End()
end