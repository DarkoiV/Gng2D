#include "Gng2D/scene/scene.hpp"
#include "Gng2D/core/log.hpp"

using Gng2D::Scene;

Scene::Scene()
{
    name = "unnamed scene";
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
}

const std::string& Scene::getName() const 
{
    return name;
}

