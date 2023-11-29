local function createRedX()
    local components = {}
    components.Sprite = {
        ['sprite'] = "red_x"
    }
    components.Transform2d = {
        ['x'] = 10.0,
        ['y'] = 11.22
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

    newEntityRecipe("RedX", components)
end

function OnEnter()
    createRedX()
    spawnEntity("RedX")
    spawnEntity("RedX")
end

function OnUpdate()

end
