
local npcs = { }

function scene_system_init()
    local ostime = os.time()
    local npc_table = content_system_get_table('npc')
    for i, props in ipairs(npc_table) do
        local npc  = actor_manager_create_actor(ostime)
        npc:SetProperties(props)    
        npc:SetProperty(PROP_HP, npc:GetMaxHP())
        npc:SetProperty(PROP_MP, npc:GetMaxMP())
        ostime = ostime + 1
    end
end

function scene_system_update()

end

function scene_system_stop()
end

