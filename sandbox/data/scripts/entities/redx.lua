HDirection = 'left'
VDirection = 'up'
PlayerControlled = false

OnAction = {};
OnAction.up = function()
    if not PlayerControlled then return end
    VDirection = 'up'
end

OnAction.down = function()
    if not PlayerControlled then return end
    VDirection = 'down'
end

OnAction.left = function()
    if not PlayerControlled then return end
    HDirection = 'left'
end

OnAction.right = function()
    if not PlayerControlled then return end
    HDirection = 'right'
end

function OnSpawn()
    if Self:hasComponent('Transform2d') == false then
        Self:addComponent('Transform2d', {})
        HDirection = nil
        VDirection = nil
        PlayerControlled = true
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
