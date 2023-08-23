#include <SDL2/SDL.h>
#include "Gng2D/core/log.hpp"
#include "Gng2D/core/window.hpp"
#include "Gng2D/scene/scene.hpp"

using Gng2D::Scene;

Scene::Scene()
{
    reg.ctx().emplace<Scene&>(*this);
}

void Scene::render(SDL_Renderer* r)
{
    entityRenderer(r);
}

void Scene::onMouseMotion(SDL_MouseMotionEvent& event)
{
    mouseSystem.motion(event);
}

void Scene::onMouseButton(SDL_MouseButtonEvent& event)
{
    mouseSystem.button(event);
}

void Scene::operator()()
{
    preUpdate();

    mouseSystem();

    update();
}

bool Scene::isPaused() const
{
    return pause;
}

bool Scene::isCompleted() const 
{
    return completed;
}

bool Scene::isKeyPressed(SDL_Scancode scancode) const
{
    const Uint8* keyboardState = SDL_GetKeyboardState(NULL);
    return keyboardState[scancode];
}

void Scene::runCoroutines()
{
    if (pause and (not runCoroutinesOnPause)) return;

    for (auto& coro : coroutines) coro();
    std::erase_if(coroutines, [](Coroutine& coro) 
    {
        return coro.isCompleted(); 
    });
}

