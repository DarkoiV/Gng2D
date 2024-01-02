Vspeed = 0;
Hspeed = 0;
Vel = 0.5;

OnAction = {};
OnAction.up = function(release)
    if release then
        Vspeed = Vspeed - Vel
    else
        Vspeed = Vspeed + Vel
    end
end

OnAction.down = function(release)
    if release then
        Vspeed = Vspeed + Vel
    else
        Vspeed = Vspeed - Vel
    end
end

OnAction.left = function(release)
    if release then
        Hspeed = Hspeed + Vel
    else
        Hspeed = Hspeed - Vel
    end
end

OnAction.right = function(release)
    if release then
        Hspeed = Hspeed - Vel
    else
        Hspeed = Hspeed + Vel
    end
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
