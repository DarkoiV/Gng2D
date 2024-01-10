local function createRedX()
    local components = {}
    components.Sprite = {
        ['sprite'] = "red_x"
    }
    components.TransformLayer = {
        ['main'] = 0,
        ['sub']  = -1
    }
    components.Info = {
        ['name'] = "RedX"
    }
    components.LuaScript = {
        ['script'] = "entities/redx"
    }
    components.Collider = {
        ['type'] = "Box",
        ['group'] = "default",
        ['width'] = 12,
        ['height'] = 12
    }
    components.Hoverable = {
        ['width'] = 11,
        ['height'] = 11,
    }

    NewEntityRecipe("RedX", components)
end

local spawnredx = function()
    SpawnEntity("RedX")
end

OnAction.cancel = function()
    if OnAction.confirm ~= spawnredx then
        OnAction.confirm = spawnredx
    else
        OnAction.confirm = nil
    end
end

OnAction.space = function()
    EmitAction('cancel')
end

function OnEnter()
    createRedX()
    SpawnEntity("RedX", { ['Transform2d'] = { ['x'] = 100 } })
    SpawnEntity("RedX", { ['Transform2d'] = { ['x'] = -100 } })
    SpawnEntity("RedX")
end

function OnUpdate()
end
