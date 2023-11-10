#include "Gng2D/scene/scene.hpp"
#include "Gng2D/commons/assert.hpp"
#include "Gng2D/components/sprite.hpp"
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

    GNG2D_ASSERT((reg.all_of<Sprite>(x1)), "FAILED TO INIT ENTITY");
    GNG2D_ASSERT((reg.all_of<Sprite>(x2)), "FAILED TO INIT ENTITY");
    GNG2D_ASSERT((reg.all_of<Sprite>(x3)), "FAILED TO INIT ENTITY");

    int counter = 0;
    reg.view<Transform2d>().each(
        [&counter, this](auto e, auto& transform)
        {
        transform.x = counter * 10;
        transform.y = -counter * 10;
        counter++;
        reg.patch<Transform2d>(e);
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
