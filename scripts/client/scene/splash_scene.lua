
local DialogAnim 
local DialogNPC 
-- WZIFEWDF 0x73D983B7 NPC对话框
--0x7F84C945  楚依恋
--[[
NPC对话框 横向居中 Y偏移290
213 193 58 NPC名字颜色
]]
function OnSceneInit()
    -- WZIFEWDF, 0x792EA858
    ui_renderer_clear()

    DialogAnim = ne_imageview_create(WZIFEWDF,  0x73D983B7 )
    DialogAnim:GetBaseSprite():SetPos(270,380)

    DialogNPC = ne_imageview_create(WZIFEWDF,  0x7F84C945 ) 
    DialogNPC:GetBaseSprite():SetPos(300,200)

    local bs = DialogAnim:GetBaseSprite()
    bs:EnableDrag(true)

    bs = DialogNPC:GetBaseSprite()
    bs:EnableDrag(true)


    local tv = ui_textview_create()
    tv:SetText('神州上下祸劫频生，灵石是否重补苍天裂痕，蚩尤的阴谋能否被挫败，三界将继续下去还是回归混沌，将由玩家英雄亲手创造。')
    tv:SetPos(300,400)
    tv:SetWidth(460)
    tv:SetColor(0xffffffff)
    tv:SetBGColor(0)
    tv:SetTextSize(17)
    
    ui_renderer_add_to_draw(DialogNPC)
    ui_renderer_add_to_draw(DialogAnim)
    ui_renderer_add_to_draw(tv)
end

function OnSceneUpdate()    
 
end

function OnSceneDraw()

end

