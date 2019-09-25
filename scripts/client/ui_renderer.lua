
function actor_on_click(actor, button, x, y)
	cxlog_info('actor_on_click',actor:GetProperty(PROP_NAME),button, x,y)
	if not actor:IsLocal() then
		local player = actor_manager_fetch_local_player()
		if player then
			player:SetTarget(actor)
		end
		npc_dialog_show(true,'神州上下祸劫频生，灵石是否重补苍天裂痕，', {
				{ 
					txt = '我是找你打架的',
					func = function()
						local player = actor_manager_fetch_local_player()
						player:StopMove()
						combat_system_clear_actors()
						combat_system_add_attacker(player)
						combat_system_add_defender(player:GetTarget())
						combat_system_switch_battle(true)
						cxlog_info(player:GetProperty(PROP_NAME), player:GetTarget():GetProperty(PROP_NAME))
					end
				},
				{ 
					txt ='相信你是冤枉的',
					func=function()
						cxlog_info('相信你是冤枉的')
					end
				},
				{ 
					txt='告辞',
					func=function()
						cxlog_info('告辞')
					end
				}
			})
	end
end

function actor_on_hover(actor, x, y)
	-- cxlog_info('actor_on_hover',actor:GetProperty(PROP_NAME),x,y)
end

function input_manager_on_mouse_move(mx, my)
	-- cxlog_info('input_manager_on_mouse_move', mx, my)
end