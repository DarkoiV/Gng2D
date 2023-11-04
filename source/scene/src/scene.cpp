#include "Gng2D/scene/scene.hpp"
#include "Gng2D/commons/log.hpp"

using Gng2D::Scene;

Scene::Scene()
{
    using namespace entt::literals;
    name = "unnamed scene";
    reg.ctx().emplace<Luna&>(luna);
    reg.ctx().emplace<Assets&>(assets);

    Assets::loadGlobalSprite("red_x");
    auto type           = componentLibrary.getMeta("Sprite");
    auto emplace        = type.func("emplace"_hs);
    auto any            = type.construct(&reg, "red_x"_hs);

    LOG::INFO("Any is", type.info().name());
    
    auto red_x = reg.create();
    emplace.invoke({}, &reg, red_x, entt::forward_as_meta(any));
    reg.emplace<Gng2D::Position>(red_x, 0.0f, 0.0f);
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

void Scene::update()
{
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

