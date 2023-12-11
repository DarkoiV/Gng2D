HDirection = 'left'
VDirection = 'up'

OnAction = {};
OnAction.Up = function()
    local transform2d = Self:getComponent('Transform2d')
    transform2d.y = transform2d.y + 1;
end

OnAction.Down = function()
    local transform2d = Self:getComponent('Transform2d')
    transform2d.y = transform2d.y - 1;
end


function OnSpawn()
    if Self:hasComponent('Transform2d') == false then
        Self:addComponent('Transform2d', {})
        HDirection = nil
        VDirection = nil
    else
        local transform = Self:getComponent('Transform2d')
        if transform.x < 0 then
            HDirection = 'right'
        else
            HDirection = 'left'
        end
    end
end

function OnUpdate()
    local transform = Self:getComponent('Transform2d')

    if HDirection == 'left' then
        transform.x = transform.x - 0.5
    elseif HDirection == 'right' then
        transform.x = transform.x + 0.5
    end

    if VDirection == 'up' then
        transform.y = transform.y + 0.5
    elseif VDirection == 'down' then
        transform.y = transform.y - 0.5
    end

    if transform.x < -314 then
        HDirection = 'right'
    end
    if transform.x > 314 then
        HDirection = 'left'
    end

    if transform.y > 194 then
        VDirection = 'down'
    end
    if transform.y < -194 then
        VDirection = 'up'
    end
end
