HDirection = 'left'
VDirection = 'up'

function OnCreate()
    local transform = Self:getComponent('Transform2d')
    if transform.x < 0 then
        HDirection = 'right'
    else
        HDirection = 'left'
    end
end

function OnUpdate()
    local transform = Self:getComponent('Transform2d')

    if HDirection == 'left' then
        transform.x = transform.x - 0.5
    else
        transform.x = transform.x + 0.5
    end

    if VDirection == 'up' then
        transform.y = transform.y + 0.5
    else
        transform.y = transform.y - 0.5
    end

    if transform.x < -308 then
        HDirection = 'right'
    end
    if transform.x > 308 then
        HDirection = 'left'
    end

    if transform.y > 188 then
        VDirection = 'down'
    end
    if transform.y < -188 then
        VDirection = 'up'
    end
end
