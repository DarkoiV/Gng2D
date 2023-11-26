#include "Gng2D/scene/scene.hpp"
#include "Gng2D/commons/imgui.hpp"
#include "Gng2D/commons/log.hpp"
#include "Gng2D/commons/repository.hpp"
#include "Gng2D/core/global.hpp"
#include "Gng2D/entities/red_x.hpp"

#ifdef GNG2D_IMGUI_ENABLED
#include "Gng2D/scene/debug/imgui_overlay.hpp"
#endif

using Gng2D::Scene;

Scene::Scene()
    : luna(Gng2D::GLOBAL::LUNA_STATE)
{
#ifdef GNG2D_IMGUI_ENABLED
    systems.emplace_back(std::make_unique<Gng2D::ImguiOverlay>(reg));
#endif
}

Scene::~Scene()
{
    reg.clear();
}

void Scene::onEnter()
{
    LOG::INFO("Entering", name);
    Repository::loadSprite("red_x");
    RedX spawner(reg);
    spawner.spawn();
    spawner.spawn();
    spawner.spawn();
}

void Scene::onExit()
{
    LOG::INFO("Exiting", name);
}

void Scene::update()
{
    for (auto& system: systems)
    {
        system->onUpdate();
    }
}

void Scene::render(SDL_Renderer* r)
{
    entityRenderer(r);
}

void Scene::onKeyPress(SDL_KeyboardEvent& e)
{
    inputHandler.handleKeyPress(e);
}

const std::string& Scene::getName() const
{
    return name;
}
