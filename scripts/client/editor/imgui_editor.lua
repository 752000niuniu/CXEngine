script_system_dofile('editor/avatar_editor.lua')
script_system_dofile('editor/enter_editor.lua')
script_system_dofile('editor/scene_editor.lua')


function on_imgui_editor_update()
    on_avatar_editor_update()
    on_enter_editor_update()
    on_scene_editor_update()
end