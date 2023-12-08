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

local function mov(entity, transform2d)
    transform2d.x = -transform2d.x
    transform2d.y = -transform2d.y
end

local counter = 0;
function OnUpdate()
    counter = counter + 1

    -- every 175 ticks flip positions
    if counter == 175 then
        ViewEach(mov, 'Transform2d')
        counter = 0
    end
end
