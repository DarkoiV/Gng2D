function OnEnter()
    SpawnEntity("redx", { ['Transform2d'] = { ['x'] = 100 } })
    SpawnEntity("redx", { ['Transform2d'] = { ['x'] = -100 } })

    RegisterOnKeyPressAction('e', 'up')
    RegisterOnKeyPressAction('d', 'down')
    RegisterOnKeyPressAction('s', 'left')
    RegisterOnKeyPressAction('f', 'right')
end
