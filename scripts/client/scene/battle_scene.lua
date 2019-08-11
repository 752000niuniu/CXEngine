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

local enemy_actor 
local self_actor
function OnSceneInit()
	local ratio_x = game_get_width()/ 640 
	local ratio_y = game_get_height()/ 480
	combat_self_pos =  calc_combat_self_pos(ratio_x, ratio_y)
	combat_enemy_pos =  calc_combat_enemy_pos(ratio_x, ratio_y)
	
    -- local s  = "Ocean藏心"
    -- scene_add_player(s,math.floor(combat_self_pos[1].x) ,math.floor(combat_self_pos[1].y),2,9,5)
    -- scene_add_pet(s.."1",math.floor(combat_enemy_pos[1].x) ,math.floor(combat_enemy_pos[1].y),0,5,1)
    -- scene_add_npc(s..tostring(1), 530,1110,1,1,1,"asd")
    -- scene_add_pet(s..tostring(2), 560,970,1,5,1)
    -- scene_add_player(s..tostring(1), 530,1110,1,56)
    -- scene_add_player(s..tostring(2), 530,1110,2,42)
    -- scene_add_player(s..tostring(3), 530,1110,3,50)
    -- scene_add_player(s..tostring(4), 530,1110,4,2)
	-- scene_set_player(s)
	cxlog_info('battle scene OnSceneInit')
	combat_system_start_battle()

	local player = actor_manager_fetch_local_player()
	if not player then
		player = actor_manager_create_player(math.tointeger( os.time()))
		player:SetName('oceacx')
		player:SetSceneID(1135)
		player:SetRoleID(16)
		player:SetWeaponID(56)
		player:SetDir(2)
		actor_manager_set_local_player(player:GetID())
	end


	self_actor = player
	self_actor:SetPos( combat_self_pos[1].x , combat_self_pos[1].y)


	enemy_actor = actor_manager_create_player(math.tointeger( os.time()) + 10 )
	enemy_actor:SetName('oceacx en')
	enemy_actor:SetSceneID(1135)
	enemy_actor:SetRoleID(16)
	enemy_actor:SetWeaponID(56)
	enemy_actor:SetDir(0)
	enemy_actor:SetPos(combat_enemy_pos[1].x , combat_enemy_pos[1].y)


	combat_system_add_actor(self_actor)
	combat_system_add_actor(enemy_actor)

	self_actor:SetActionID(ActionBatidle)
	enemy_actor:SetActionID(ActionBatidle)

	combat_system_start_battle()
	
	
end

function OnSceneUpdate()
	combat_system_update()
end

function OnSceneDraw()
	combat_system_draw()
end