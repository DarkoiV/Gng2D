#include "Gng2D/scene/scene.hpp"
#include "Gng2D/commons/log.hpp"

using Gng2D::Scene;

Scene::Scene()
{
    name = "unnamed scene";
    reg.ctx().emplace<Luna&>(luna);
    reg.ctx().emplace<Assets&>(assets);

    Assets::loadGlobalSprite("red_x");
    
    auto red_x = reg.create();
    reg.emplace<Gng2D::Sprite>(red_x, *assets.getSprite("red_x"));
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

