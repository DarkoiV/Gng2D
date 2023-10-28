#include "Gng2D/scene/scene.hpp"
#include "Gng2D/commons/log.hpp"

using Gng2D::Scene;

Scene::Scene()
{
    name = "unnamed scene";
}

Scene::~Scene() 
{
    reg.clear();
}

static void LogPress(entt::entity, Gng2D::KeyPress::Action action)
{
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

