local BattleBG

local tAttackers = {}
local tDefenders = {}

local combat_self_pos = {}
local combat_enemy_pos = {}

function calc_combat_self_pos(ratio_x, ratio_y)
	return {
		{ x = 415.0 * ratio_x, y = 275.0 * ratio_y },
		{ x = 355.0 * ratio_x, y = 305.0 * ratio_y },
		{ x = 475.0 * ratio_x, y = 245.0 * ratio_y },
		{ x = 295.0 * ratio_x, y = 335.0 * ratio_y },
		{ x = 535.0 * ratio_x, y = 215.0 * ratio_y },
		{ x = 465.0 * ratio_x, y = 315.0 * ratio_y },
		{ x = 405.0 * ratio_x, y = 345.0 * ratio_y },
		{ x = 525.0 * ratio_x, y = 285.0 * ratio_y },
		{ x = 345.0 * ratio_x, y = 375.0 * ratio_y },
		{ x = 585.0 * ratio_x, y = 255.0 * ratio_y },
	}
end

function calc_combat_enemy_pos(ratio_x, ratio_y)
	return {
		{ x = 175.0 * ratio_x, y = 170.0 * ratio_y },   
		{ x = 115.0 * ratio_x, y = 200.0 * ratio_y },   
		{ x = 235.0 * ratio_x, y = 140.0 * ratio_y },   
		{ x = 55.0  * ratio_x, y = 230.0 * ratio_y },  
		{ x = 295.0 * ratio_x, y = 110.0 * ratio_y },   
		{ x = 220.0 * ratio_x, y = 210.0 * ratio_y },   
		{ x = 160.0 * ratio_x, y = 240.0 * ratio_y },   
		{ x = 280.0 * ratio_x, y = 180.0 * ratio_y },   
		{ x = 100.0 * ratio_x, y = 270.0 * ratio_y },   
		{ x = 340.0 * ratio_x, y = 150.0 * ratio_y },   
	}
end

function combat_system_init()
   	BattleBG = animation_create(ADDONWDF, 0xE3B87E0F)
end

function combat_system_update()
	
end

function combat_system_draw()
	BattleBG:Draw()
end

function combat_system_on_start()
	
end

function combat_system_on_end()

end

function combat_system_imgui_update()
	imgui.Begin('Menu',menu_show)

    if imgui.Button('法术##player') then

    end

    if imgui.Button('特技##player') then

    end

    if imgui.Button('道具##player') then

    end

    if imgui.Button('防御##player') then

    end

    if imgui.Button('召唤##player') then

    end

    if imgui.Button('召还##player') then

    end

    if imgui.Button('捕捉##player') then

    end

    if imgui.Button('逃跑##player') then
        combat_system_switch_battle(false)
    end

    if imgui.Button('进入战斗') then
        combat_system_switch_battle(true)
    end

    imgui.End()

    imgui.Begin('BBMenu',bb_menu_show)
    if imgui.Button('法术##bb') then

    end

    if imgui.Button('道具##bb') then

    end

    if imgui.Button('防御##bb') then

    end

    if imgui.Button('逃跑##bb') then

    end
    imgui.End()

    if  imgui.KeysMod('ALT') and imgui.IsKeyReleased(string.byte('A'))  then
        local player = actor_manager_fetch_local_player()
        player:GetTarget():SetProperty(PROP_ASM_BEHIT_ANIM, res_encode(ADDONWDF,0x1D3FF13C))
        player:PlayAttack()
    end
end

