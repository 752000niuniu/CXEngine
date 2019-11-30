function scene_system_init()
    local ostime = os.time()
    playerBB  = actor_manager_create_actor(ostime)
    playerBB:SetProperties({
        [PROP_ACTOR_TYPE] = ACTOR_TYPE_SUMMON,
        [PROP_AVATAR_ID] = '鬼将',
        [PROP_WEAPON_AVATAR_ID] = '',
        [PROP_NAME] ='鬼将',
        [PROP_POS] = {563,376},
        [PROP_SUMMON_ATK_QUAL] = 1400,
        [PROP_SUMMON_DEF_QUAL] = 1400,
        [PROP_SUMMON_HEALTH_QUAL] = 4500,
        [PROP_SUMMON_MAGIC_QUAL] =  2500,
        [PROP_SUMMON_SPEED_QUAL] = 1200,
        [PROP_SUMMON_DODGE_QUAL] =  1200,
        [PROP_SUMMON_GROW_COEF] =  1.25,
        [PROP_BASE_HEALTH] = 417 ,
        [PROP_BASE_MAGIC] = 680 ,
        [PROP_BASE_FORCE] = 171,
        [PROP_BASE_STAMINA] =  171 ,
        [PROP_BASE_AGILITY] = 171,
        [PROP_LV] = 151,
        [PROP_SCENE_ID] = -105,
    })
    playerBB:SetProperty(PROP_HP, playerBB:GetMaxHP())
    playerBB:SetProperty(PROP_MP, playerBB:GetMaxMP())
    

end

function scene_system_update()

end

function scene_system_stop()
end

