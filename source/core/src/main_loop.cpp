#include "Gng2D/core/main_loop.hpp"
#include "Gng2D/commons/imgui.hpp"
#include "Gng2D/commons/log.hpp"
#include "Gng2D/core/global.hpp"

using Gng2D::MainLoop;
using namespace Gng2D::GLOBAL;

void MainLoop::operator()()
{
    IMGUI_NEW_FRAME();
    while (APP_RUNNING)
        loop();
    if (CURRENT_SCENE) CURRENT_SCENE->onExit();
}

void MainLoop::loop()
{
    if (NEXT_SCENE) switchScene();
    if (not CURRENT_SCENE) LOG::FATAL("No scene set");

    uint64_t currentTS  = SDL_GetTicks64();
    uint32_t elapsed    = currentTS - previousTS;
    previousTS          = currentTS;
    logicLag           += elapsed;

    eventsProcessing();
    while (logicLag >= LOGIC_TICK)
    {
        logicProcessing();
        logicLag -= LOGIC_TICK;
    }
    rendering();
}

static void transformCordinates(int& x, int& y)
{
#ifdef GNG2D_IMGUI_ENABLED
    x -= IMGUI_LEFT_SIDEBAR;
#endif
    {
        const int xoff = Gng2D::GLOBAL::RENDER_WIDTH / 2;
        const int yoff = Gng2D::GLOBAL::RENDER_HEIGHT / 2;

        x /= Gng2D::GLOBAL::RENDER_SCALE;
        y /= Gng2D::GLOBAL::RENDER_SCALE;
        x -= xoff;
        y -= yoff;
        y  = y;
    }
}

void MainLoop::eventsProcessing()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        IMGUI_PROCESS_EVENT(event);
        switch (event.type)
        {
        case SDL_QUIT:
            APP_RUNNING = false;
            break;
        case SDL_KEYDOWN:
            if (event.key.repeat) break;
            CURRENT_SCENE->onKeyPress(event.key);
            break;
        case SDL_KEYUP:
            CURRENT_SCENE->onKeyRelease(event.key);
            break;
        case SDL_MOUSEMOTION:
            transformCordinates(event.motion.x, event.motion.y);
            CURRENT_SCENE->onMouseMotion(event.motion);
            break;
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            CURRENT_SCENE->onMouseButton(event.button);
            break;
        }
    }
}

void MainLoop::logicProcessing()
{
    IMGUI_NEW_FRAME();
    CURRENT_SCENE->update();
    IMGUI_END_FRAME();
}

void MainLoop::rendering()
{
    CURRENT_SCENE->render(RENDERER);
    IMGUI_RENDER_FRAME();
    SDL_RenderPresent(RENDERER);
    SDL_RenderClear(RENDERER);
}

void MainLoop::switchScene()
{
    LOG::INFO("Switching scene");
    if (CURRENT_SCENE)
    {
        CURRENT_SCENE->onExit();
    }

    CURRENT_SCENE = std::move(NEXT_SCENE);
    CURRENT_SCENE->onEnter();
    previousTS = SDL_GetTicks64();
}
