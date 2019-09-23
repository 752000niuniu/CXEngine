
function actor_on_click(actor, button, x, y)
	cxlog_info('actor_on_click',actor:GetProperty(PROP_NAME),button, x,y)
	if not actor:IsLocal() then
		local player = actor_manager_fetch_local_player()
		if player then
			player:SetTarget(actor)
		end
	end
end

function actor_on_hover(actor, x, y)
	-- cxlog_info('actor_on_hover',actor:GetProperty(PROP_NAME),x,y)
end

function input_manager_on_mouse_move(mx, my)
	-- cxlog_info('input_manager_on_mouse_move', mx, my)
end