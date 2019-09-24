
function actor_on_click(actor, button, x, y)
	cxlog_info('actor_on_click',actor:GetProperty(PROP_NAME),button, x,y)
	if not actor:IsLocal() then
		local player = actor_manager_fetch_local_player()
		if player then
			player:SetTarget(actor)
		end
		npc_dialog_show_options('我是找你打架的','相信你是冤枉的','告辞')
		-- npc_dialog_show(true,'神州上下祸劫频生，灵石是否重补苍天裂痕，蚩尤的阴谋能否被挫败，三界将继续下去还是回归混沌，将由玩家英雄亲手创造。神州上下祸劫频生，灵石是否重补苍天裂痕，蚩尤的阴谋能否被挫败，三界将继续下去还是回归混沌，将由玩家英雄亲手创造。神州上下祸劫频生，灵石是否重补苍天裂痕，蚩尤的阴谋能否被挫败，三界将继续下去还是回归混沌，将由玩家英雄亲手创造。神州上下祸劫频生，灵石是否重补苍天裂痕，蚩尤的阴谋能否被挫败，三界将继续下去还是回归混沌，将由玩家英雄亲手创造。')
	end
end

function actor_on_hover(actor, x, y)
	-- cxlog_info('actor_on_hover',actor:GetProperty(PROP_NAME),x,y)
end

function input_manager_on_mouse_move(mx, my)
	-- cxlog_info('input_manager_on_mouse_move', mx, my)
end