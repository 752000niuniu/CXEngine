
local Battles = {}

function combat_system_create_battle(attackers, defenders)
	local battle = {}
	battle.attackers = attackers
	battle.defenders = defenders
	table.insert(Battles,battle)
end

function combat_system_remove_battle(battle_id)
	
end

function combat_system_update_battle()
	for i,battle in ipairs(Battles) do
		
	end
end


--玩家A发起对玩家B的战斗
stub[PTO_C2S_START_BATTLE]= function(req)
	
end



--战斗系统更新
function combat_system_update()

end