#include "Gng2D/scene/scene.hpp"
#include "Gng2D/commons/log.hpp"
#include "Gng2D/commons/repository.hpp"
#include "Gng2D/entities/red_x.hpp"

using Gng2D::Scene;

Scene::Scene() { }

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
