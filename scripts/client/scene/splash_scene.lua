
local DialogAnim 
local DialogNPC 
-- WZIFEWDF 0x73D983B7 NPC对话框
--0x7F84C945  楚依恋
--[[
NPC对话框 横向居中 Y偏移290
213 193 58 NPC名字颜色
]]

animation = animation or nil
local player
function OnSceneInit()
    -- WZIFEWDF, 0x792EA858
    ui_renderer_clear()

    -- DialogAnim = ne_imageview_create(WZIFEWDF,  0x73D983B7 )
    -- DialogAnim:GetBaseSprite():SetPos(270,380)

    -- DialogNPC = ne_imageview_create(WZIFEWDF,  0x7F84C945 ) 
    -- DialogNPC:GetBaseSprite():SetPos(300,200)

    -- local bs = DialogAnim:GetBaseSprite()
    -- bs:EnableDrag(true)

    -- bs = DialogNPC:GetBaseSprite()
    -- bs:EnableDrag(true)
    
    -- local tv = ui_textview_create()
    -- tv:SetText('神州上下祸劫频生，灵石是否重补苍天裂痕，蚩尤的阴谋能否被挫败，三界将继续下去还是回归混沌，将由玩家英雄亲手创造。')
    -- tv:SetPos(300,400)
    -- tv:SetWidth(460)
    -- tv:SetColor(0xffffffff)
    -- tv:SetBGColor(0)
    -- tv:SetTextSize(17)
    
    -- ui_renderer_add_to_draw(DialogNPC)
    -- ui_renderer_add_to_draw(DialogAnim)
    -- ui_renderer_add_to_draw(tv)
    animation = animation_create(12,0XA16A06FF)

    

    animation:SetPos(500,100)

    player = actor_manager_create_actor(2)
    player:SetProperties({
        [PROP_AVATAR_ID] = 'JXK-SWORD',
        [PROP_WEAPON_AVATAR_ID] = 'JXK-SWORD-060-X',
        [PROP_NAME] ='剑侠客',
        [PROP_POS] = {616,412},
        [PROP_BASE_HEALTH] =  333 ,
        [PROP_BASE_MAGIC] = 157 ,
        [PROP_BASE_FORCE] =  689,
        [PROP_BASE_STAMINA] = 215 ,
        [PROP_BASE_AGILITY] =  157,
        [PROP_LV] =  145
    })
end

function OnSceneUpdate()    
    animation:Update()

end

function OnSceneImGuiUpdate()
    if imgui.Button('Translate') then
        animation:Translate(40,100,500, function()
            local x,y = animation:GetPos()
            animation:SetPos(x+40,y+100)
            animation:Translate(-40,-100,500, function()
                local x,y = animation:GetPos()
                animation:SetPos(x-40,y-100)
            end)
        end)
    end

    if imgui.Button('playAttack') then
        player:PlayAttack()
    end
end

function OnSceneDraw()
    animation:Draw()
end

