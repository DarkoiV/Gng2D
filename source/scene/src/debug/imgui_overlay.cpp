#include "Gng2D/scene/debug/imgui_overlay.hpp"
#include "Gng2D/commons/imgui.hpp"
#include "Gng2D/commons/log.hpp"
#include <SDL2/SDL.h>

using Gng2D::ImguiOverlay;

ImguiOverlay::ImguiOverlay(entt::registry& r)
    : SystemInterface(r)
{
    LOG::INFO("ImguiOverlay created");
}

void ImguiOverlay::onUpdate()
{
    if (ImGui::Begin("EntityView"))
    {
        auto view = reg.view<entt::entity>();
        for (auto [e]: view.each())
        {
            ImGui::Text("Entity %05d", (uint32_t)e);
        }
    }
    ImGui::End();
}

bool ImguiOverlay::catchKeyPress(SDL_KeyboardEvent& e)
{
    switch (e.keysym.sym)
    {
    case SDLK_F2:
        return true;
    }
    return false;
}
