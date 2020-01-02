function npc_on_show_dialog(player, target)
	npc_dialog_show(true,'神州上下祸劫频生，灵石是否重补苍天裂痕，', {
			{ 
				txt = '我是找你打架的',
				func = function()
					local msg = {}
					msg.atk = player:GetID()
					msg.def = actor:GetID()
					net_send_message(PTO_C2S_COMBAT_START, cjson.encode(msg))

					local player = actor_manager_fetch_local_player()
					player:StopMove()
				end
			},
			{ 
				txt ='组队',
				func=function()
					player:AddTeamMember(target)
				end
			},
			{ 
				txt='踢出队伍',
				func=function()
					player:RemoveTeamMember(target)
				end
			},
			{ 
				txt='战斗',
				func=function()
					local req = {}
					req.atk = player:GetID()
					req.def = target:GetID()
					net_send_message(PTO_C2S_COMBAT_START, cjson.encode(req))
				end
			},
			{
				txt='取消',
				func=function()
					
				end
			}
		})
end