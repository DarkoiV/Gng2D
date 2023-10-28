#include "Gng2D/scene/scene.hpp"
#include "Gng2D/core/log.hpp"

// TMP INCLUDES
#include "Gng2D/components/layer.hpp"
#include "Gng2D/components/sprite.hpp"
#include "Gng2D/components/position.hpp"
#include "Gng2D/core/global.hpp"
#include <SDL2/SDL_image.h>
//

using Gng2D::Scene;
using namespace entt::literals;

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
    switch(action)
    {
        case "up"_hs:
            Gng2D::LOG::INFO("UP");
            break;
        case "left"_hs:
            Gng2D::LOG::INFO("LEFT");
            break;
        case "down"_hs:
            Gng2D::LOG::INFO("DOWN");
            break;
        case "right"_hs:
            Gng2D::LOG::INFO("RIGHT");
            break;
    }
}

void Scene::onEnter()
{
    LOG::INFO("Entering", name);
    auto x_entity = reg.create();
    auto* texture = IMG_LoadTexture(GLOBAL::RENDERER, std::string(GLOBAL::APP_DIRECTORY + "/red_x.png").c_str());
    if (not texture) LOG::FATAL(IMG_GetError());
    auto& pos       = reg.emplace<Position>(x_entity, 0.0f, 0.0f);
    auto& sprite    = reg.emplace<Sprite>(x_entity, texture, SDL_Rect{0, 0, 12, 12});
    auto& key       = reg.emplace<KeyPress>(x_entity);
    entt::sink{key.signal}.connect<&LogPress>();
    key.actions.emplace_back("up"_hs);
    key.actions.emplace_back("left"_hs);
    key.actions.emplace_back("down"_hs);
    key.actions.emplace_back("right"_hs);
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

