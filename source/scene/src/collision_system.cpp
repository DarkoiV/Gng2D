#include "Gng2D/scene/collision_system.hpp"
#include "Gng2D/components/collider.hpp"
#include "Gng2D/components/transform.hpp"
#include <SDL2/SDL_rect.h>
#include <algorithm>

using namespace Gng2D;

CollisionSystem::CollisionSystem(entt::registry& r)
    : SystemInterface(r)
{
}

void CollisionSystem::onUpdate()
{
    auto view = reg.view<BoxCollider, detail::Position>();
    for (auto&& [e1, collider1, pos1]: view.each())
    {
        std::for_each(view.begin(), view.end(),
                      [&](auto e2)
        {
            if (e1 == e2) return;
            auto& pos2      = view.get<detail::Position>(e2);
            auto& collider2 = view.get<BoxCollider>(e2);

            SDL_FRect rect1 = {.x = pos1.x - (float)collider1.width / 2.f,
                               .y = pos1.y - (float)collider1.width / 2.f,
                               .w = (float)collider1.width,
                               .h = (float)collider1.height};
            SDL_FRect rect2 = {.x = pos2.x - (float)collider2.width / 2.f,
                               .y = pos2.y - (float)collider2.width / 2.f,
                               .w = (float)collider2.width,
                               .h = (float)collider2.height};
            if (SDL_HasIntersectionF(&rect1, &rect2)) LOG::TRACE("COLLISION!");
        });
    }
}
