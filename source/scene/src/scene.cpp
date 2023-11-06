#include "Gng2D/scene/scene.hpp"
#include "Gng2D/commons/log.hpp"
#include "Gng2D/entities/entity_recipe.hpp"

using Gng2D::Scene;

Scene::Scene()
{
    using namespace entt::literals;
    name = "unnamed scene";
    reg.ctx().emplace<Luna&>(luna);
    reg.ctx().emplace<Assets&>(assets);

    Assets::loadGlobalSprite("red_x");

    auto redXRecipe = EntityRecipe::redXRecipe(reg);
    auto x1         = redXRecipe.spawn();
    auto x2         = redXRecipe.spawn();
    auto x3         = redXRecipe.spawn();

    int counter = 0;
    reg.view<TransformPosition>().each(
        [&counter, this](auto e, auto& tpos)
        {
        tpos.x = counter * 10;
        tpos.y = -counter * 10;
        counter++;
        reg.patch<TransformPosition>(e);
    });
}

Scene::~Scene()
{
    reg.clear();
}

void Scene::onEnter()
{
    LOG::INFO("Entering", name);
}

void Scene::onExit()
{
    LOG::INFO("Exiting", name);
}

void Scene::update() { }

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
