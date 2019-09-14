script_system_dofile('editor/actor_editor.lua')
script_system_dofile('editor/scene_editor.lua')

function on_imgui_editor_update()
    on_scene_editor_update()
    on_actor_editor_update()
end