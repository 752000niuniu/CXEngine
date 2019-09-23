local BattleBG

function combat_system_init()
   BattleBG  = animation_create(ADDONWDF, 0xE3B87E0F)

end

function combat_system_update()

end

function combat_system_draw()
	BattleBG:Draw()
end