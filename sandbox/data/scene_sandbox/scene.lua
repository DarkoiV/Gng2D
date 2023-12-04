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

    NewEntityRecipe("RedX", components)
end

function OnEnter()
    createRedX()
    SpawnEntity("RedX", { ['Transform2d'] = { ['x'] = 100 } })
    SpawnEntity("RedX", { ['Transform2d'] = { ['x'] = -100 } })
    SpawnEntity("RedX")
end

function OnUpdate()

end
