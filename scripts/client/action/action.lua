_skill_template_table = _skill_template_table or {}
local skill_template_table = _skill_template_table

local ActionHandles = {}

ActionHandles[ASM_ACTION] = {
    OnEnter = function(actor)

    end,
    OnExit = function(actor)

    end
}


ActionHandles[ASM_ATTACK_ACTION]={
    OnEnter = function(actor)
        
    end,
    OnExit = function(actor)

    end
}



ActionHandles[ASM_CAST_ACTION]={
    OnEnter = function(actor)
    
    end,
    OnExit = function(actor)

    end
}

ActionHandles[ASM_BEHIT_ACTION]={
    OnEnter = function(actor)
        -- local dead = math.random(0,10) < 3
        -- actor:SetProperty(PROP_IS_DEAD, dead)

        -- local randoms= {}
        -- table.insert(randoms, math.random(10,99))
        -- table.insert(randoms, math.random(100,999))
        -- table.insert(randoms, math.random(1000,9999))
        -- table.insert(randoms, math.random(10000,99999))
        -- table.insert(randoms, math.random(100000,999999))
        
        -- local which = math.random(1,#randoms)
        -- actor:SetProperty(PROP_ASM_DAMAGE,randoms[which]) 
    end,
    OnExit = function(actor)

    end
}


ActionHandles[ASM_BECAST_ACTION]={
    OnEnter = function(actor)
        local dead = math.random(0,1) == 0
        actor:SetProperty(PROP_IS_DEAD, dead)

        local randoms= {}
        table.insert(randoms, math.random(10,99))
        table.insert(randoms, math.random(100,999))
        table.insert(randoms, math.random(1000,9999))
        table.insert(randoms, math.random(10000,99999))
        table.insert(randoms, math.random(100000,999999))
        
        local which = math.random(1,#randoms)
        actor:SetProperty(PROP_ASM_DAMAGE,randoms[which]) 

    end,
    OnExit = function(actor)

    end
}


ActionHandles[ASM_DEAD_FLY_ACTION]={
    OnEnter = function(actor)

    end,
    OnExit = function(actor)

    end
}

ActionHandles[ASM_PATH_MOVE_ACTION]={
    OnEnter = function(actor)

    end,
    OnExit = function(actor)

    end
}

function asm_action_on_enter(actor, type)
    if ActionHandles[type] then
        ActionHandles[type].OnEnter(actor)
    end
end

function asm_action_on_exit(actor, type)
    if ActionHandles[type] then
        ActionHandles[type].OnExit(actor)
    end
end

function asm_system_init()
    skill_template_table =  content_system_get_table('skill') 
end

