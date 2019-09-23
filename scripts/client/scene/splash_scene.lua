
local DialogAnim 
function OnSceneInit()
    -- WZIFEWDF, 0x792EA858
    DialogAnim = ne_imageview_create(WZIFEWDF, 0xEF073E43 )
    local bs = DialogAnim:GetBaseSprite()
    bs:EnableDrag(true)
    ui_renderer_add_to_draw(DialogAnim)
end

function OnSceneUpdate()    
 
end

function OnSceneDraw()
 
end

