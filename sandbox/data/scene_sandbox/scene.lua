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
    components.BoxCollider = {
        ['width'] = 12,
        ['height'] = 12
    }

    NewEntityRecipe("RedX", components)
end

function OnEnter()
    createRedX()
    SpawnEntity("RedX", { ['Transform2d'] = { ['x'] = 100 } })
    SpawnEntity("RedX", { ['Transform2d'] = { ['x'] = -100 } })
    SpawnEntity("RedX")
end

local function mov(entity)
    if entity:hasComponent('Transform2d') then
        local transform = entity:getComponent('Transform2d')
        if transform.x > 0 then
            transform.x = transform.x - 0.25
        end
    end
end

function OnUpdate()
    ViewEach(mov)
end
