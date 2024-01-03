Vspeed = 0;
Hspeed = 0;
Vel = 0.5;

OnAction = {};
OnAction.up = function()
    Vspeed = Vspeed + Vel
end

OnAction.down = function()
    Vspeed = Vspeed - Vel
end

OnAction.left = function()
    Hspeed = Hspeed - Vel
end

OnAction.right = function()
    Hspeed = Hspeed + Vel
end

OnAction.rUp = function()
    Vspeed = Vspeed - Vel
end

OnAction.rDown = function()
    Vspeed = Vspeed + Vel
end

OnAction.rLeft = function()
    Hspeed = Hspeed + Vel
end

OnAction.rRight = function()
    Hspeed = Hspeed - Vel
end

OnHover = function()
    transform = Self:getComponent('Transform2d')
    transform.x = transform.x + 10
end

function OnSpawn()
    if Self:hasComponent('Transform2d') == false then
        Self:addComponent('Transform2d', {})
        Vspeed = 0
        Hspeed = 0
    end
end

function OnUpdate()
    local transform = Self:getComponent('Transform2d')
    transform.x = transform.x + Hspeed;
    transform.y = transform.y + Vspeed;
end
