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

    NewEntityRecipe("RedX", components)
end

function OnEnter()
    createRedX()
    SpawnEntity("RedX", { ['Transform2d'] = { ['x'] = 100 } })
    SpawnEntity("RedX", { ['Transform2d'] = { ['x'] = -100 } })
    SpawnEntity("RedX")
end

local counter = 60;
local ammount = 0;
function OnUpdate()
    if counter == 0 and ammount < 6000 then
        SpawnEntity("RedX", { ['Transform2d'] = { ['x'] = 100 } })
        SpawnEntity("RedX", { ['Transform2d'] = { ['x'] = -100 } })
        ammount = ammount + 2
        counter = 60
    end
    if counter > 0 then counter = counter - 1 end
end
