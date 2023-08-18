#pragma once
#include <cmath>
#include <entt/entity/registry.hpp>
#include <SDL2/SDL.h>
#include "Gng2D/components/colider.hpp"
#include "Gng2D/components/position.hpp"

namespace Gng2D
{
template <CircleColliderType Collider>
struct ColliderRenderer
{
    ColliderRenderer(entt::registry& r)
        : reg(r)
    {
    }

    void drawCircle(SDL_Renderer* r, int radius, int x, int y)
    {
        for (int w = 0; w <= radius * 2; w++)
        {
            for (int h = 0; h <= radius * 2; h++)
            {
                int dx = radius - w;
                int dy = radius - h;
                if ((dx*dx + dy*dy) <= (radius * radius))
                {
                    SDL_RenderDrawPoint(r, x + dx, y + dy);
                }
            }
        }   
    }

    void operator()(SDL_Renderer* renderer)
    {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 175);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        for (const auto& [_, collider, position] : reg.view<Collider, Position>().each())
        {
            drawCircle(renderer, collider.radius, position.x, position.y);
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    };

private:
    entt::registry& reg;
};
}

