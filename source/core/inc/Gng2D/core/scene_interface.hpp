#pragma once
#include <string>
#include <SDL2/SDL.h>

namespace Gng2D
{
struct SceneInterface 
{
    SceneInterface()                        = default;
    SceneInterface(const SceneInterface&)   = delete;
    SceneInterface(SceneInterface&&)        = delete;
    virtual ~SceneInterface()               = default;

    auto operator=(SceneInterface&)     = delete;
    auto operator=(SceneInterface&&)    = delete;

    virtual void onEnter()              = 0;
    virtual void onExit()               = 0;
    virtual void update()               = 0;
    virtual void render(SDL_Renderer*)  = 0;

    virtual void onKeyboardEvent(SDL_KeyboardEvent&) = 0;

    virtual const std::string& getName() const  = 0;
};
}

