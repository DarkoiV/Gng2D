#pragma once
#include <SDL2/SDL.h>
#include <memory>
#include <string>

namespace Gng2D {
struct SceneInterface
{
    SceneInterface()                      = default;
    SceneInterface(const SceneInterface&) = delete;
    SceneInterface(SceneInterface&&)      = delete;
    virtual ~SceneInterface()             = default;

    auto operator=(SceneInterface&)  = delete;
    auto operator=(SceneInterface&&) = delete;

    virtual void onEnter()             = 0;
    virtual void onExit()              = 0;
    virtual void update()              = 0;
    virtual void render(SDL_Renderer*) = 0;

    virtual void onKeyPress(SDL_KeyboardEvent&)       = 0;
    virtual void onMouseMotion(SDL_MouseMotionEvent&) = 0;
    virtual void onMouseButton(SDL_MouseButtonEvent&) = 0;

    virtual const std::string& getName() const = 0;
};
using ScenePtr = std::unique_ptr<SceneInterface>;
} // namespace Gng2D
